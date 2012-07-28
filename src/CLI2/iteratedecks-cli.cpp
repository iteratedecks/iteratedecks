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

int mainWithObjects(unsigned int const & numberOfIterations
                   ,ActiveDeck const & deck1
                   ,ActiveDeck const & deck2
                   ,int const achievementIndex
                   )
{
	AchievementIndex = achievementIndex; // index, not id
	bConsoleOutput = false;
	
	RESULTS r;
	for (UINT k=0;k<numberOfIterations;k++)	{
		ActiveDeck X(deck1);
		ActiveDeck Y(deck2);


		Simulate(X,Y,r);
	}
	std::cout << r.Win << "/" << r.Games << std::endl;
	
	return 0;
}


/******************************************************************************
 ******************************************************************************
 * The main function, little bit higher interface                             *
 *                                                                            *
 * We use use the simple arguments provided to construct objects.             *
 * They will then be passed to mainWithObjects                                *
 ******************************************************************************
 ******************************************************************************/

int mainWithOptions(unsigned int const & numberOfIterations
                   ,bool const & firstDeckIsOrdered
                   ,char const * const & _deck1
                   ,char const * const & _deck2
                   ,int const & achievementIndex
                   )
{
	DB.LoadAchievementXML("achievements.xml");
	DB.LoadCardXML("cards.xml");
	DB.LoadMissionXML("missions.xml");
	DB.LoadRaidXML("raids.xml");
	// That does something strange, there is an unresolved bug.
	DB.LoadQuestXML("quests.xml");

	ActiveDeck deck1(_deck1, DB.GetPointer());
	deck1.SetOrderMatters(firstDeckIsOrdered);
	ActiveDeck deck2(_deck2, DB.GetPointer());
	
	return mainWithObjects(numberOfIterations, deck1, deck2, achievementIndex);
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
 * Default values.
 */
#define DEFAULT_NUMBER_OF_ITERATIONS 1000;

/**
 * Errors
 */
#define E_NO_SUCH_OPTION -1
#define E_INCORRECT_ARGUMENT -2
#define E_NOT_TWO_DECKS -3

/**
 * Option table
 */
static option const long_options[] = 
    { { "number-of-iterations" , required_argument, 0, 'n' }
    , { "first-deck-is-ordered", no_argument      , 0, 'o' }
    , { "achievement-index"       , required_argument, 0, 'a' }
    };
static char const * const short_options = "n:o";

int main(int const argc, char * const * const argv)
{
    unsigned int numberOfIterations = DEFAULT_NUMBER_OF_ITERATIONS;
    int achievementIndex = -1;
    bool firstDeckIsOrdered = false;

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
                    ssNumberOfIterations >> numberOfIterations;
                    if(ssNumberOfIterations.fail()) {
                        std::cerr << "--number-of-iterations requires an integer argument" << std::endl;
                        return E_INCORRECT_ARGUMENT;
                    }
                } break;
            case 'o':
                    firstDeckIsOrdered = true;
                break;
            case 'a': {
                    stringstream ssAchievementIndex(optarg);
                    ssAchievementIndex >> achievementIndex;
                    if(ssAchievementIndex.fail()) {
                        std::cerr << "--achievement-index requires an integer argument" << std::endl;
                        return E_INCORRECT_ARGUMENT;
                    }
                } break;
            case '?':
                return E_NO_SUCH_OPTION;
            default: {
                    std::cerr << "default: " << c << std::endl;
                }
        }
    }
    
    char const * deck1;
    char const * deck2;
    // other arguments, we expect exactly two decks
    if(optind+2 == argc) {
        deck1 = argv[optind+0];
        deck2 = argv[optind+1];
    } else {
        std::cerr << "please specify exactly two decks to test" << std::endl;
        return E_NOT_TWO_DECKS;
    }    
    return mainWithOptions(numberOfIterations, firstDeckIsOrdered, deck1, deck2, achievementIndex);
}
