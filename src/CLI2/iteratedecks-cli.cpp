/*
 * Platform seperation macros.
 * After this block, the macros __windows__ __linux__ or __mac__ should be defined.
 */
// TODO: Could some mac user add an appropriate define here
#ifdef _WIN32
    #define __windows__
#endif
#ifdef _WIN64
    #define __windows__
#endif
// __linux__ is defined by gcc on gnu/linux
// __FreeBSD__ is defined by gcc on FreeBSD


/*
 * The platform specific includes.
 */
#if defined(__windows__)
    #include <tchar.h>
#elif defined(__linux__) || defined(__FreeBSD__)
    #include <string>
    #include <unistd.h>
    #include <iostream>
#else
    #error "Platform undefined."
#endif


/*
 * Common includes
 */
// c++
#include <iostream>
#include <sstream>
#include <iostream>

// common includes
#include <ctime>
#include <stdio.h>

// here come NETRAT's evil hacks
#include "../EXE/deck.h"
#include "../EXE/cards.h"
//#include "../EXE/branches.h"
#include "../EXE/simulate.h"

#include "verify.hpp"
#include "cliOptions.hpp"
#include "cliParser.hpp"

#include <cfloat>

#define assertDE(a,b) assert(a-DBL_EPSILON < b && b < a+DBL_EPSILON)
#define assertDEE(a,b,c) assert(abs(a-b) <= c)

using namespace EvaluateDecks::CLI;

/**
 * @see http://en.wikipedia.org/wiki/Binomial_coefficient
 */
double binomialCoefficient(unsigned int const n
                          ,unsigned int k
                          )
{
    assert(k <= n);
    if (n == 0) {
        return 0;
    } else if (2*k > n) {
        return binomialCoefficient(n,n-k);
    } else if (k == 0) {
        return 1;
    } else if (k == 1) {
        return n;
    }
    double result = 1;
    for(unsigned int i = 1; i <= k; i++) {
        unsigned long const nom = n-k+i;
        unsigned long const denom = i;
        result = result * nom / denom;
    }
    //std::clog << "BinCo(" << n << "," << k << ") = ";
    //std::clog << result << std::endl;
    return result;
}

/**
 * x = B(X = k | p,n )
 */
double Bin(double const p
          ,unsigned int const k
          ,unsigned int const n
          )
{
    assert(0 <= p);
    assert(p <= 1);
    assert(k <= n);
    double const q(1-p);
    double const binCo(binomialCoefficient(n,k));
    assert(binomialCoefficient(12,6) == 924);
    assert(binomialCoefficient(11,5) == 462);
    if(0 < k && k < n) {
        double const _bc1 = binomialCoefficient(n-1,k);
        double const _bc2 = binomialCoefficient(n-1,k-1);
        double const _bc = _bc1 + _bc2;
        assert(abs(binCo / _bc - 1) <= 1e-12);
    }
    double const x = binCo * pow(p,k) * pow(q,n-k);
    assert(x>=0);
    assert(x<=1);
    if(p >= 0.1) {
        //std::clog << "Bin(X = " << k << "|" << p << "," << n << ") = " << x << std::endl;
    }
    return x;
}

/**
 * x = B(X <= k | p,n)
 * @return x
 */
double cumBin(double const p
             ,unsigned int const k
             ,unsigned int const n
             )
{
    assertDE(Bin(0.125,12,12),1.4551915228366851807E-11);
    assertDE(Bin(0.125,6,12),1.5819048858247697353E-3);

    double sumL = 0.0d;
    double sumU = 0.0d;
    for(unsigned int i = 0; i <= n; i++) {
        if (i <= k) {
            sumL += Bin(p,i,n);
        } else {
            sumU += Bin(p,i,n);
        }
    }
    if(p >= 0.01) {
        //std::clog << "Bin(X <= " << k << "|" << p << "," << n << ") = " << sumL << std::endl;
    }
    if(1 < sumL && sumL <= 1 + 1e-6) {
        sumL == 1;
    }
    if(1 < sumU && sumU <= 1 + 1e-6) {
        sumU == 1;
    }
    assert(sumL>=0-DBL_EPSILON);
    assert(sumL<=1+DBL_EPSILON);
    assert(sumU>=0-DBL_EPSILON);
    assert(sumU<=1+DBL_EPSILON);
    double sum = sumL + sumU;
    //std::clog << "Total sum of probabilites for Bin( true |" <<  p << "," << n << ") = " << sum << std::endl;
    //assertDE(sum,1);

    // we normalize this result by sum, which should be 1, but probably is not exactly one
    return sumL / sum;
}

/**
 * x = B(X <= k | p,n )
 * where B is the Binomial distribution.
 * @return p
 */
double cumBinInv(double const x
                ,unsigned int const k
                ,unsigned int const n
                )
{
    assert(k <= n);
    assert(0 <= x);
    assert(x <= 1);

    assertDEE(cumBin(0.5,9,12),0.9807128906,1e-8);

    // use search...
    double pl(0);
    double pu(1);

    double const eps(DBL_EPSILON);
    while (pl + eps < pu) {
        double const pm = (pl+pu)/2;
        double const testX(cumBin(pm,k,n));

        if(x < testX) {
            // test value is larged as target value
            // cumBin is monotonic decreasing in p
            // we want to decrease test value, thus we need to increase argument
            pl = pm;
        } else if (x > testX) {
            pu = pm;
        } else {
            pl = pm;
            pu = pm;
        }

        //std::clog << "approximating cumBinInv(" << x << ", " << k << ", " << n << ") ";
        //std::clog << "pm=" << pm << " ";
        //std::clog << "testX=" << testX << " ";
        //std::clog << "delta=" << (x-testX) << std::endl;
    }
    double const pm = (pl+pu)/2;
    assertDEE(cumBin(pm,k,n),x,1e-12);
    return pm;
}

/**
 * p, such that B(X >= k   | p,n ) = alpha/2   (a small value)
 * p, such that B(X <  k   | p,n ) = 1-alpha/2 (a large value)
 * p, such that B(X <= k-1 | p,n ) = 1-alpha/2 (a large value)
 * where B is the Binomial distribution.
 */
double lowerBound(unsigned int const k  //< wins
                 ,unsigned int const n  //< games
                 ,double const gamma    //< confidence level
                 )
{
    assert(k <= n);
    assert(0 <= gamma);
    assert(gamma <= 1);
    double const alpha(1-gamma);
    if (k > 0) {
        return cumBinInv(1-alpha/2,k-1,n);
    } else {
        return 0.0d;
    }
}

/**
 * p, such that B(X <= k | p,n ) = alpha/2
 * where B is the Binomial distribution.
 */
double upperBound(unsigned int const k  //< successfull results
                 ,unsigned int const n  //< total tries
                 ,double const gamma    //< confidence level
                 )
{
    assert(k <= n);
    assert(0 <= gamma);
    assert(gamma <= 1);
    double const alpha(1-gamma);
    if (k < n) {
        return cumBinInv(alpha/2,k,n);
    } else {
        return 1.0d;
    }
}


RESULTS simulate(ActiveDeck const & deck1
                ,ActiveDeck const & deck2
                ,DeckLogger & attackLogger
                ,DeckLogger & defenseLogger
                ,SimulationLogger & simulationLogger
                ,unsigned int const & numberOfIterations
                )
{
    // Here comes the actual simulations
	RESULTS r;
    for (UINT k=0;k<numberOfIterations;k++)	{
        ActiveDeck X(deck1);
        X.logger = &attackLogger;
        ActiveDeck Y(deck2);
        Y.logger = &defenseLogger;

        simulationLogger.simulationStart(k);
        Simulate(X,Y,r,&simulationLogger);
        simulationLogger.simulationEnd(k);
    }

    // Print result
    std::cout << "Games won: " << std::setw(11) << r.Win << "/" << std::setw(11) << r.Games << " ";
    std::cout.flush();
	double const winRate = (double)r.Win / (double)r.Games; // estimator
    std::cout << "estimator=" << std::setiosflags(ios::fixed) << std::setprecision(4) << winRate << " ";
    std::cout.flush();
    double const gamma(0.95); // confidence level
    double const lBound(lowerBound(r.Win,r.Games,gamma));
    double const uBound(upperBound(r.Win,r.Games,gamma));
    std::cout << "confidence [" << lBound << ";" << uBound << "]";
    assert(0 <= lBound);
    assert(lBound <= winRate);
    assert(winRate <= uBound);
    assert(uBound <= 1);
    std::cout << "; ";
    double const averageNetPoints ((double)r.Points / (double)r.Games);
    std::cout << "ANP=" << averageNetPoints;
    std::cout << std::endl;
    return r;
}

/**
 * The main function, little bit higher interface
 *
 * We use use the simple arguments provided to construct objects.
 * They will then be passed to mainWithObjects
 */
int mainWithOptions(CliOptions const & options
                   )
{
    // only help, then we are quick
    if (options.printHelpAndExit) {
        printUsage();
        return 0;
    }

    // so we do a simulation, well we need some data files first
	DB.LoadAchievementXML("achievements.xml");
	DB.LoadCardXML("cards.xml");
	DB.LoadMissionXML("missions.xml");
	DB.LoadRaidXML("raids.xml");
	DB.LoadQuestXML("quests.xml");

    // construct the decks
    // TODO own function for this.
    // ... first deck
    assert(options.attackDeck.getType() == DeckArgument::HASH);
    // FIXME: should not pass c_str
	ActiveDeck deck1(options.attackDeck.getHash().c_str(), DB.GetPointer());
	deck1.SetOrderMatters(options.attackDeck.isOrdered());

    // ... second deck
    assert(options.defenseDeck.getType() == DeckArgument::HASH);
	ActiveDeck deck2(options.defenseDeck.getHash().c_str(), DB.GetPointer());
    deck2.SetOrderMatters(options.defenseDeck.isOrdered());

    // set random generator seed. Note that if there was no argument
    // given we will set to 1, which is equivalent to not calling srand
	srand(options.seed);

    // TODO need to discover what this does
	AchievementIndex = (int)(options.achievementOptions); // index, not id

    // TODO will replace console output by better logging system sometime
	bConsoleOutput = (options.verbosity > 0);

    // Set up logging
    // TODO add support for more fine grained logging control
    unsigned int const loggingFlags ((options.verbosity) > 0 ? Logger::LOG_ALL : Logger::LOG_NONE);
    Logger logger(loggingFlags,&DB);
    logger.setColorMode(options.colorMode);
    DeckLogger attackLogger(DeckLogger::ATTACK, logger);
    DeckLogger defenseLogger(DeckLogger::DEFENSE, logger);
    SimulationLogger simulationLogger(logger);

    RESULTS r;
    r = simulate(deck1,deck2,attackLogger,defenseLogger,simulationLogger,options.numberOfIterations);

    // Do verify, if requested.
    VerifyOptions const & verifyOptions(options.verifyOptions);
    if(verifyOptions.doVerify) {
        double const winRate = (double)r.Win / (double)r.Games;
        if(verifyOptions.min <= winRate && winRate <= verifyOptions.max) {
            // pass
            return 0;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

/**
 * Errors
 */
#define E_RUNTIME_ERROR -1;
#define E_LOGIC_ERROR -2;

using namespace EvaluateDecks::CLI;

/**
 * Reads cli arguments, parses them with parseCliOptions, then calls mainWithOptions().
 */
int main(int const argc, char * const * const argv)
{
    CliOptions options = parseCliOptions(argc, argv);

    try {
        return mainWithOptions(options);
    } catch(std::runtime_error const & e) {
        std::cerr << "Runtime error:" << std::endl;
        std::cerr << e.what() << std::endl;
        return E_RUNTIME_ERROR;
    } catch(std::logic_error const & e) {
        std::cerr << "Logic error:" << std::endl;
        std::cerr << e.what() << std::endl;
        return E_LOGIC_ERROR;
    }
}
