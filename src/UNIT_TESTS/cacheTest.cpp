//#define BOOST_TEST_MODULE cache test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../CORE/iterateDecksCore.hpp"
#include "../CORE/autoDeckTemplate.hpp"
#include "../CORE/exceptions.hpp"
#include "../CACHE/diskBackedCache.hpp"

using namespace IterateDecks::Core;
using namespace IterateDecks::Cache;

BOOST_AUTO_TEST_CASE( test_simulator_cache ) { try {

    IterateDecksCore::Ptr core = IterateDecksCore::Ptr(new IterateDecksCore());
    DiskBackedCache::Ptr cache = DiskBackedCache::Ptr(new DiskBackedCache(core));
    
    SimulationTaskClass task;

    task.minimalNumberOfGames = 1;
    task.surge = false;
    std::list<unsigned int> ids;
    ids.push_back(1000u);
    ids.push_back(16u);
    task.attacker = AutoDeckTemplate::Ptr(new AutoDeckTemplate(ids));
    task.defender = task.attacker;
    
    Result result = cache->simulate(task);

    BOOST_CHECK(result.numberOfGames >= 1);
    BOOST_CHECK_EQUAL(result.gamesWon, result.numberOfGames);

    task.surge = true;
    Result result2 = cache->simulate(task);
    BOOST_CHECK(result2.numberOfGames >= 1);
    BOOST_CHECK_EQUAL(result2.gamesWon, 0);

} catch (Exception &e) { std::cerr << e.what() << std::endl; e.printStacktrace(std::cerr); throw e; } }
