#include "runCommand.hpp"
#include "../CORE/iterateDecksCore.hpp"
#include "../CACHE/diskBackedCache.hpp"
#include "../OPT/praetorianMutator.hpp"
#include "../OPT/praetorianOptimizer.hpp"
#include "../CORE/assert.hpp"
#include "../CORE/Logger.hpp"

using namespace IterateDecks::Cache;
using namespace IterateDecks::Opt;

namespace IterateDecks {
    namespace CLI3 {

        RunCommand::RunCommand(int verbosity
                              ,bool dontReadCache
                              )
        : optimizeAttacker(false)
        , optimizeDefender(false)
        {
            IterateDecksCore::Ptr idSim = IterateDecksCore::Ptr (new IterateDecksCore());
            SimulatorCore::Ptr sim = idSim;
            DiskBackedCache::Ptr cache = DiskBackedCache::Ptr(
                new DiskBackedCache(sim)
            );
            cache->setDontReadCache(dontReadCache);
            this->simulator = cache;
            CardDB const & cardDB = idSim->getCardDB();
            PraetorianMutator::Ptr mutator = PraetorianMutator::Ptr(new PraetorianMutator(cardDB));
            this->optimizer = Optimizer::Ptr(new PraetorianOptimizer(cache, mutator));

            // logging stuff
            unsigned long const loggingFlags ((verbosity) > 0 ? Logger::LOG_ALL : Logger::LOG_NONE);
            if (loggingFlags != Logger::LOG_NONE) {
                std::clog << "Enabling logging." << std::endl;
                this->logger = new Logger(loggingFlags, cardDB);
                this->logger->setColorMode(Logger::COLOR_NONE);
                idSim->setLogger(this->logger);
            } else {
                this->logger = NULL;
            }
        }

        RunCommand::~RunCommand()
        {
            if (this->logger != NULL) {
                delete this->logger;
                this->logger = NULL;
            }
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
