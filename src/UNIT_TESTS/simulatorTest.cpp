#define BOOST_TEST_MODULE cache_test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../CORE/iterateDecksCore.hpp"
#include "../CORE/autoDeckTemplate.hpp"
#include "../CORE/cardDB.hpp"
#include "../CORE/exceptions.hpp"

BOOST_AUTO_TEST_CASE( test_simulator_direct ) { try {

    IterateDecksCore::Ptr simulator = IterateDecksCore::Ptr(new IterateDecksCore());
    CardDB cardDB;

    SimulationTaskClass task;

    task.minimalNumberOfGames = 1000;
    task.surge = false;
    std::list<unsigned int> ids;
    ids.push_back(1000u);
    ids.push_back(16u);
    task.attacker = AutoDeckTemplate::Ptr(new AutoDeckTemplate(ids));
    task.defender = task.attacker;
    
    Result result = simulator->simulate(task);

    BOOST_CHECK_EQUAL(result.numberOfGames, 1000);
    BOOST_CHECK_EQUAL(result.gamesWon, 1000);

} catch (Exception &e) { std::cerr << e.what() << std::endl; e.printStacktrace(std::cerr); throw e; } }
