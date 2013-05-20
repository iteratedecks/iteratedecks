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
            IterateDecksCore::Ptr idSim = IterateDecksCore::Ptr (new IterateDecksCore());
            SimulatorCore::Ptr sim = idSim;
            DiskBackedCache::Ptr cache = DiskBackedCache::Ptr(
                new DiskBackedCache(sim)
            );
            this->simulator = cache;
            CardDB const & cardDB = idSim->getCardDB();
            PraetorianMutator::Ptr mutator = PraetorianMutator::Ptr(new PraetorianMutator(cardDB));
            this->optimizer = Optimizer::Ptr(new PraetorianOptimizer(cache, mutator));
        }

        int RunCommand::execute() {

            

            


            assertX(!(this->optimizeAttacker && this->optimizeDefender));
            if(this->optimizeAttacker || this->optimizeDefender) {

                DeckTemplate::Ptr optimizedDeck;
                if (this->optimizeAttacker) {
                    optimizedDeck = this->optimizer->optimizeMany(this->task, true);
                } else {
                    optimizedDeck = this->optimizer->optimizeMany(this->task, false);
                }
                std::cout << "Optimization finished, found:" << std::endl;
                std::cout << optimizedDeck->toString() << std::endl;
                return 0;
            } else {            
                Result result = this->simulator->simulate(this->task);
                std::cout << result.gamesWon << " / " << result.numberOfGames << std::endl;
                return 0;
            }

        }

        void
        RunCommand::abort()
        {
            this->simulator->abort();
            this->optimizer->abort();
        }
        
    }
}
