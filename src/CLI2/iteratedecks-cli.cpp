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

#include <getopt.h>

// here come NETRAT's evil hacks
#include "../EXE/deck.h"
#include "../EXE/cards.h"
//#include "../EXE/branches.h"
#include "../EXE/simulate.h"

#include "verify.hpp"
#include "cliOptions.hpp"

int mainWithObjects(unsigned int const & numberOfIterations
                   ,ActiveDeck const & deck1
                   ,ActiveDeck const & deck2
                   ,int const achievementIndex
                   ,VerifyOptions const & verifyOptions
                   ,unsigned int const & verbosity
                   )
{
	AchievementIndex = achievementIndex; // index, not id
	bConsoleOutput = (verbosity > 0);
	
	RESULTS r;
	for (UINT k=0;k<numberOfIterations;k++)	{
		ActiveDeck X(deck1);
		ActiveDeck Y(deck2);


		Simulate(X,Y,r);
	}
	double winRate = (double)r.Win / (double)r.Games;
	std::cout << r.Win << "/" << r.Games << std::endl;
	
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
    deck1.SetOrderMatters(options.defenseDeck.isOrdered());
	
	return mainWithObjects(options.numberOfIterations, deck1, deck2, options.achievementOptions, options.verifyOptions, options.verbosity);
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

/**
 * Option table
 */
static option const long_options[] =
    { { "number-of-iterations" , required_argument, 0, 'n' }
    , { "first-deck-is-ordered", no_argument      , 0, 'o' }
    , { "achievement-index"    , required_argument, 0, 'a' }
    , { "verify"               , required_argument, 0 , 0 }
    , { "verbose"              , no_argument      , 0 , 'v' }
    };
static char const * const short_options = "n:oa:v";

int main(int const argc, char * const * const argv)
{
    CliOptions options;

    // gnu getopt stuff
    while(true) {
        int option_index = 0;
        int c = getopt_long(argc, argv, short_options, long_options, &option_index);
        if(c == -1) {
            break;
        }
  
        switch(c) {
            case 'n': {
                    stringstream ssNumberOfIterations(optarg);
                    ssNumberOfIterations >> options.numberOfIterations;
                    if(ssNumberOfIterations.fail()) {
                        std::cerr << "-n --number-of-iterations requires an integer argument" << std::endl;
                        return E_INCORRECT_ARGUMENT;
                    }
                } break;
            case 'o': {
                    if (options.attackDeck.getType() == DeckArgument::HASH) {
                        options.attackDeck.setOrdered(true);
                    } else {
                        std::cerr << "ordered deck only makes sense for hash decks";
                        return E_INCORRECT_ARGUMENT;
                    }
                } break;
            case 'a': {
                    stringstream ssAchievementIndex(optarg);
                    int achievementIndex;
                    ssAchievementIndex >> achievementIndex;
                    if(ssAchievementIndex.fail()) {
                        std::cerr << "-a --achievement-index requires an integer argument" << std::endl;
                        return E_INCORRECT_ARGUMENT;
                    }
                    if (achievementIndex >= 0) {
                        options.achievementOptions.enableCheck(achievementIndex);
                    } else {
                        options.achievementOptions.disableCheck();
                    }
                } break;
            case 'v': {
                    options.verbosity++;
                } break;
            case '?':
                return E_NO_SUCH_OPTION;
            case 0:
                switch(option_index) {
                    case 3:
                            options.verifyOptions = VerifyOptions(optarg);
                        break;
                    default:
                            std::cerr << "0 default: " << option_index << std::endl;
                } break;
            default: {
                    std::cerr << "default: " << c << std::endl;
                }
        }
    }
    
    // other arguments, we expect exactly two decks
    if(optind+2 == argc) {
        options.attackDeck.setHash(argv[optind+0]);
        options.defenseDeck.setHash(argv[optind+1]);
    } else {
        std::cerr << "please specify exactly two decks to test" << std::endl;
        return E_NOT_TWO_DECKS;
    }
    try {
        return mainWithOptions(options);
    } catch(std::runtime_error const & e) {
        std::cerr << "Runtime error:" << std::endl;
        std::cerr << e.what() << std::endl;
        return E_RUNTIME_ERROR;
    }
}
