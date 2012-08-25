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

// common includes
#include <stdio.h>
#include <time.h>

// here come NETRAT's evil hacks
#include "../EXE/deck.h"
#include "../EXE/cards.h"
//#include "../EXE/branches.h"
#include "../EXE/simulate.h"

#include "verify.hpp"
#include "cliOptions.hpp"
#include "cliParser.hpp"

using namespace EvaluateDecks::CLI;

int mainWithObjects(unsigned int const & numberOfIterations
                   ,ActiveDeck const & deck1
                   ,ActiveDeck const & deck2
                   ,int const achievementIndex
                   ,unsigned int const & verbosity
                   ,CliOptions const & options
                   )
{
    srand(options.seed);
	AchievementIndex = achievementIndex; // index, not id
	bConsoleOutput = (verbosity > 0);

    unsigned int loggingFlags = (verbosity) > 0 ? Logger::LOG_ALL : Logger::LOG_NONE;
    Logger logger(loggingFlags,&DB);
    logger.setColorMode(options.colorMode);
    DeckLogger attackLogger(DeckLogger::ATTACK, logger);
    DeckLogger defenseLogger(DeckLogger::DEFENSE, logger);
    SimulationLogger simulationLogger(logger);

	RESULTS r;
	for (UINT k=0;k<numberOfIterations;k++)	{
		ActiveDeck X(deck1);
        X.logger = &attackLogger;
		ActiveDeck Y(deck2);
        Y.logger = &defenseLogger;

		Simulate(X,Y,r,&simulationLogger);
	}
	double winRate = (double)r.Win / (double)r.Games;
	std::cout << r.Win << "/" << r.Games << std::endl;

    VerifyOptions const & verifyOptions(options.verifyOptions);
	if(verifyOptions.doVerify) {
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


/******************************************************************************
 ******************************************************************************
 * The main function, little bit higher interface                             *
 *                                                                            *
 * We use use the simple arguments provided to construct objects.             *
 * They will then be passed to mainWithObjects                                *
 ******************************************************************************
 ******************************************************************************/

int mainWithOptions(CliOptions const & options
                   )
{
	DB.LoadAchievementXML("achievements.xml");
	DB.LoadCardXML("cards.xml");
	DB.LoadMissionXML("missions.xml");
	DB.LoadRaidXML("raids.xml");
	DB.LoadQuestXML("quests.xml");

    // FIXME: should not pass c_str
    assert(options.attackDeck.getType() == DeckArgument::HASH);
	ActiveDeck deck1(options.attackDeck.getHash().c_str(), DB.GetPointer());
	deck1.SetOrderMatters(options.attackDeck.isOrdered());

    assert(options.defenseDeck.getType() == DeckArgument::HASH);
	ActiveDeck deck2(options.defenseDeck.getHash().c_str(), DB.GetPointer());
    deck2.SetOrderMatters(options.defenseDeck.isOrdered());
	
	return mainWithObjects(options.numberOfIterations, deck1, deck2, options.achievementOptions, options.verbosity, options);
}

/******************************************************************************
 ******************************************************************************
 * The main function, cli interface                                           *
 *                                                                            *
 * We use gnu getopt_long for option passing, they are defined in             *
 * long_options and short_options.                                            *
 * After passing the options, we call the next main method, mainWithOptions   *
 * with the values from the options or the default values.                    *
 ******************************************************************************
 ******************************************************************************/


/**
 * Errors
 */
#define E_NO_SUCH_OPTION -1
#define E_INCORRECT_ARGUMENT -2
#define E_NOT_TWO_DECKS -3
#define E_RUNTIME_ERROR -4;

using namespace EvaluateDecks::CLI;

int main(int const argc, char * const * const argv)
{
    CliOptions options = parseCliOptions(argc, argv);

    try {
        return mainWithOptions(options);
    } catch(std::runtime_error const & e) {
        std::cerr << "Runtime error:" << std::endl;
        std::cerr << e.what() << std::endl;
        return E_RUNTIME_ERROR;
    }
}
