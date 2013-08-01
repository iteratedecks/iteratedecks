#include "runCommand.hpp"
#include "../CORE/iterateDecksCore.hpp"
#include "../CACHE/diskBackedCache.hpp"
#include "../OPT/praetorianMutator.hpp"
#include "../OPT/praetorianOptimizer.hpp"
#include "../CORE/assert.hpp"
#include "../CORE/Logger.hpp"
#include <fstream>
#include "ownedCardsParser.hpp"
#include "../CACHE/multiDeckDecompositor.hpp"

using namespace IterateDecks::Cache;
using namespace IterateDecks::Opt;
using namespace IterateDecks::Core;

namespace IterateDecks {
    namespace CLI3 {

        RunCommand::RunCommand(int verbosity
                              ,bool dontReadCache
                              ,unsigned int extraCards
                              ,Core::CardDB const & cardDB
                              )
        : optimizeAttacker(false)
        , optimizeDefender(false)
        , optimizationTarget(OptimizationTarget::WINRATE)
        , verbosity(verbosity)
        {
            // the core
            IterateDecksCore::Ptr idSim = IterateDecksCore::Ptr (new IterateDecksCore(cardDB));
            SimulatorCore::Ptr sim = idSim;
            // the cache
            DiskBackedCache::Ptr cache = DiskBackedCache::Ptr(
                new DiskBackedCache(sim)
            );
            cache->setDontReadCache(dontReadCache);
            // the decompositor
            MultiDeckDecompositor::Ptr decompositor = MultiDeckDecompositor::Ptr (new MultiDeckDecompositor(cache));
            this->simulator = decompositor;

            PraetorianMutator::Ptr mutator;
            // owned cards?
            std::ifstream ownedCardsStream("cards.txt");
            if (ownedCardsStream.is_open()) {
                std::multiset<unsigned int> ownedCards = parseSimpleSeparatedString(
                     ownedCardsStream
                    ,"\n"
                    ,":"
                    );

                mutator = PraetorianMutator::Ptr(new PraetorianMutator(cardDB, ownedCards, extraCards));
            } else {
                mutator = PraetorianMutator::Ptr(new PraetorianMutator(cardDB));
            }
            this->optimizer = Optimizer::Ptr(new PraetorianOptimizer(cache, mutator));

            // logging stuff
            unsigned long const loggingFlags ((this->verbosity) > 0 ? Logger::LOG_ALL : Logger::LOG_NONE);
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

        int RunCommand::execute(Core::CardDB const & cardDB) {
            assertX(!(this->optimizeAttacker && this->optimizeDefender));
            if(this->optimizeAttacker || this->optimizeDefender) {

                DeckTemplate::Ptr optimizedDeck;
                if (this->optimizeAttacker) {
                    optimizedDeck = this->optimizer->optimizeMany(this->task, true, this->optimizationTarget);
                } else {
                    optimizedDeck = this->optimizer->optimizeMany(this->task, false, this->optimizationTarget);
                }
                std::cout << "Optimization finished, found:" << std::endl;
                std::cout << optimizedDeck->toString() << std::endl;
                return 0;
            } else {
                Result r = this->simulator->simulate(this->task);
                std::cout << "Games played:                    " << std::setw(11) << r.numberOfGames
                          << std::endl;
                std::cout << "Games won:                       " << std::setw(11) << r.gamesWon
                          << " " << r.getWinRate() << std::endl;
                std::cout << "Games lost:                      " << std::setw(11) << r.gamesLost
                          << " " << r.getLossRate() << std::endl;
                std::cout << "Games stalled:                   " << std::setw(11) << r.gamesStalled
                          << " " << r.getStallRate() << std::endl;
                std::cout << "Points for attacker on manual: " << std::setw(13) << r.pointsAttacker
                          << " " << r.getManualANPAttacker() << std::endl;
                std::cout << "Points for attacker on auto:   " << std::setw(13) << r.pointsAttackerAuto
                          << " " << r.getAutoANPAttacker() << std::endl;
                std::cout << "Points for defender on manual: " << std::setw(13) << r.pointsDefender
                          << " " << r.getManualANPDefender() << std::endl;
                std::cout << "Points for defender on auto:   " << std::setw(13) << r.pointsDefenderAuto
                          << " " << r.getAutoANPDefender() << std::endl;
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
