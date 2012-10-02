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
#include "cliSimulate.hpp"
#include "../EXE/assert.hpp"
#include <stdexcept>

using namespace EvaluateDecks::CLI;

/**
 * The main function, little bit higher interface
 *
 * We use use the simple arguments provided to construct objects.
 * They will then be passed to mainWithObjects
 */
int mainWithOptions(CliOptions const & options
                   )
{
    // only help, then we quit
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

    // construct the decks
    // TODO own function for this.
    // ... first deck
    assert(options.attackDeck.getType() == DeckArgument::HASH);
    // FIXME: should not pass c_str
    ActiveDeck deck1(options.attackDeck.getHash().c_str(), DB.GetPointer());
    deck1.SetOrderMatters(options.attackDeck.isOrdered());

    RESULTS r;

    switch(options.defenseDeck.getType()) {
    case DeckArgument::HASH: {
        // ... second deck
        ActiveDeck deck2(options.defenseDeck.getHash().c_str(), DB.GetPointer());
        deck2.SetOrderMatters(options.defenseDeck.isOrdered());
        r = simulate(deck1,deck2,attackLogger,defenseLogger,simulationLogger,options.numberOfIterations,options.surge);
                             } break;

    case DeckArgument::RAID_ID: {
        r = simulateRaid(deck1, options.defenseDeck.getRaidId(), attackLogger, simulationLogger, options.numberOfIterations);
                                } break;
    }

    printResults(r);

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
    } catch(AssertionError const & e) {
        std::cerr << "Assertion failed:" << std::endl;
        std::cerr << e.what() << std::endl;
        e.printStacktrace(std::cerr);
        return E_LOGIC_ERROR;
    } catch(RuntimeError const & e) {
        std::cerr << "Runtime error:" << std::endl;
        std::cerr << e.what() << std::endl;
        e.printStacktrace(std::cerr);
        return E_RUNTIME_ERROR;
    } catch(LogicError const & e) {
        std::cerr << "Logic error:" << std::endl;
        std::cerr << e.what() << std::endl;
        e.printStacktrace(std::cerr);
        return E_LOGIC_ERROR;
    } catch(std::runtime_error const & e) {
        std::cerr << "Runtime error (deprecated!):" << std::endl;
        std::cerr << e.what() << std::endl;
        return E_RUNTIME_ERROR;
    } catch(std::logic_error const & e) {
        std::cerr << "Logic error (deprecated!):" << std::endl;
        std::cerr << e.what() << std::endl;
        return E_LOGIC_ERROR;
    }
}
