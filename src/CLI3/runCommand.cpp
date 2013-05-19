#include "runCommand.hpp"
#include "../CORE/iterateDecksCore.hpp"
#include "../CACHE/diskBackedCache.hpp"
#include "../OPT/praetorianMutator.hpp"
#include "../OPT/praetorianOptimizer.hpp"
#include "../CORE/assert.hpp"

using namespace IterateDecks::Cache;
using namespace IterateDecks::Opt;

namespace IterateDecks {
    namespace CLI3 {

        RunCommand::RunCommand()
        : optimizeAttacker(false)
        , optimizeDefender(false)
        {
        }

        int RunCommand::execute() {

            IterateDecksCore::Ptr simulator = IterateDecksCore::Ptr(new IterateDecksCore());

            SimulatorCore::Ptr simulator2 = simulator;
            DiskBackedCache::Ptr cache = DiskBackedCache::Ptr(
                new DiskBackedCache(simulator2)
            );

            assertX(!(this->optimizeAttacker && this->optimizeDefender));
            if(this->optimizeAttacker || this->optimizeDefender) {
                CardDB const & cardDB = simulator->getCardDB();
                PraetorianMutator::Ptr mutator = PraetorianMutator::Ptr(new PraetorianMutator(cardDB));
                PraetorianOptimizer::Ptr optimizer = PraetorianOptimizer::Ptr(new PraetorianOptimizer(cache, mutator));

                DeckTemplate::Ptr optimizedDeck;
                if (this->optimizeAttacker) {
                    optimizedDeck = optimizer->optimizeMany(this->task, true);
                } else {
                    optimizedDeck = optimizer->optimizeMany(this->task, false);
                }
                std::cout << "Optimization finished, found:" << std::endl;
                std::cout << optimizedDeck->toString() << std::endl;
                return 0;
            } else {            
                Result result = cache->simulate(this->task);
                std::cout << result.gamesWon << " / " << result.numberOfGames << std::endl;
                return 0;
            }

        }
        
    }
}
