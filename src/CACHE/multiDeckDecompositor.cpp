#include "multiDeckDecompositor.hpp"
#include "../CORE/multiDeckTemplate.hpp"
#include <memory>
#include "../CORE/assert.hpp"
#include <unordered_map>
#include <boost/foreach.hpp>
#include <boost/range/adaptor/map.hpp>
#include <iostream>

using namespace IterateDecks::Core;
namespace IterateDecks {
    namespace Cache {

        MultiDeckDecompositor::MultiDeckDecompositor(SimulatorCore::Ptr delegate)
        : delegate(delegate)
        {
        }

        MultiDeckDecompositor::~MultiDeckDecompositor()
        {
        }

        IterateDecks::Core::SimulatorCore::Ptr
        MultiDeckDecompositor::getDelegate() const
        {
            return this->delegate;
        }
        
        std::string
        MultiDeckDecompositor::getCoreName() const
        {
            return this->delegate->getCoreName();
        }
        
        std::string
        MultiDeckDecompositor::getCoreVersion() const
        {
            return this->delegate->getCoreVersion();
        }

        std::string
        MultiDeckDecompositor::getCoreVersionHumanReadable() const
        {
            return this->delegate->getCoreVersionHumanReadable();
        }

        SimulationTaskClass
        taskWithIterationsAndDeck
            (SimulationTaskClass const & original
            ,unsigned long numberOfIterations
            ,DeckTemplate::Ptr const & deck
            )
        {
            SimulationTaskClass copy(original);
            copy.minimalNumberOfGames = numberOfIterations;
            copy.defender = deck;
            return copy;
        }

        typedef std::multiset<DeckTemplate::Ptr> DeckMSet;
        typedef std::pair<Result, size_t> ResultCountPair;
        typedef std::map<DeckTemplate::Ptr, ResultCountPair> ResultMap;

        Result &
        operator*=
            (Result & result
            ,double factor
            )
        {
            result.numberOfGames *= factor;
            result.gamesWon *= factor;
            result.gamesStalled *= factor;
            result.gamesLost *= factor;
            result.pointsAttacker *= factor;
            result.pointsAttackerAuto *= factor;
            result.pointsDefender *= factor;
            result.pointsDefenderAuto *= factor;
            return result;
        }

        Result operator* (Result result, double factor) {
            return result *= factor;
        }

        Result
        MultiDeckDecompositor::simulate
            (SimulationTaskClass const & task
            )
        {
            assertX(task.defender);
            if (MultiDeckTemplate::Ptr defender =
                std::dynamic_pointer_cast<MultiDeckTemplate>(task.defender)
               )
            {
                assertX(defender);
                unsigned long const numberOfIterations = task.minimalNumberOfGames;
                DeckMSet const & decks = defender->getDecks();
                ResultMap results;
                size_t const totalCount = decks.size();
                DeckMSet::const_iterator iter = decks.begin();
                while(iter != decks.end()) {
                    DeckTemplate::Ptr const currentDeck = *iter;
                    size_t const currentDeckCount = decks.count(currentDeck);
                    //std::clog << "current deck: " << currentDeck->toString() << " cardinality " << currentDeckCount << std::endl;
                    unsigned long currentIterations = static_cast<unsigned int>(std::ceil(
                                                        static_cast<double>(numberOfIterations)
                                                        * currentDeckCount
                                                        / totalCount
                                                      ));
                    SimulationTaskClass currentTask = taskWithIterationsAndDeck(task, currentIterations, currentDeck);
                    Result currentResult = this->simulate(currentTask);
                    results[currentDeck] = ResultCountPair(currentResult, currentDeckCount);
                    iter = decks.upper_bound(currentDeck);
                    //throw LogicError("Not implemented: Next deck!");
                }
                // combine results
                // . find min number
                unsigned long minIterationsForSingleItem = numberOfIterations;
                BOOST_FOREACH(ResultCountPair & resultPair, results | boost::adaptors::map_values) {
                    unsigned long currentIterationsForSingleItem = resultPair.first.numberOfGames / resultPair.second;
                    if (currentIterationsForSingleItem < minIterationsForSingleItem) {
                        minIterationsForSingleItem = currentIterationsForSingleItem;
                    }
                }
                // . combine now
                Result combinedResult;
                BOOST_FOREACH(ResultCountPair & resultPair, results | boost::adaptors::map_values) {
                    Result const & currentResult = resultPair.first;
                    size_t const count = resultPair.second;
                    unsigned long const currentIterationsForSingleItem = currentResult.numberOfGames / count;
                    double const factor = static_cast<double>(minIterationsForSingleItem)
                                        / static_cast<double>(currentIterationsForSingleItem);
                    //std::clog << "scaling result with factor " << factor << std::endl;
                    Result scaled = currentResult * factor;
                    combinedResult += scaled;
                }
                return combinedResult;
            } else {
                return this->delegate->simulate(task);
            }
        }

        void
        MultiDeckDecompositor::abort() {
            this->delegate->abort();
        }


    }
}

