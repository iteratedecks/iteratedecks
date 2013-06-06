#include "praetorianOptimizer.hpp"
#include "../CORE/assert.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace IterateDecks {
    namespace Opt {

        PraetorianOptimizer::PraetorianOptimizer(SimulatorCore::Ptr const & simulator
                                                ,Mutator::Ptr const & mutator
                                                )
        : core(simulator)
        , mutator(mutator)
        , aborted(false)
        {
        }

        SimulationTaskClass withDeck(SimulationTaskClass const & orig
                                    ,bool attacker
                                    ,DeckTemplate::Ptr deck
                                    )
        {
            SimulationTaskClass task(orig);
            if (attacker) {
                task.attacker = deck;
            } else {
                task.defender = deck;
            }
            return task;
        }

        SimulationTaskClass
        withIterationsAndDeck(SimulationTaskClass const & orig
                             ,bool attacker
                             ,DeckTemplate::Ptr deck
                             ,unsigned int iterations
                             )
        {
            SimulationTaskClass task(orig);
            if (attacker) {
                task.attacker = deck;
            } else {
                task.defender = deck;
            }
            task.minimalNumberOfGames = iterations;
            return task;
        }

        struct Compare {
            std::map<DeckTemplate::Ptr, Result> const results;

            Compare(std::map<DeckTemplate::Ptr, Result> const & results)
            : results(results)
            {
            }
            
            bool operator() (DeckTemplate::Ptr const &a, DeckTemplate::Ptr const & b) {
                try {
                    Result resultA = results.at(a);
                    Result resultB = results.at(b);
                    return resultA.getWinRate() > resultB.getWinRate();
                } catch (std::out_of_range &e) {
                    std::stringstream ssMessage;
                    ssMessage << "caught a std::out_of_range: " << e.what();
                    ssMessage << " that means that one of the decks is not there";
                    throw LogicError(ssMessage.str(), 1);
                }
            }
        };

        void
        PraetorianOptimizer::simpleCrop(SimulationTaskClass const & originalTask
                                       ,std::vector<DeckTemplate::Ptr> & decks
                                       ,double factor
                                       ,unsigned long totalNumberOfIterations
                                       ,unsigned long minimalNumberOfIterationsEach
                                       ,bool optimizeAttacker
                                       )
       {
            size_t const initialCount = decks.size();
            assertGT(initialCount,1u);
            assertGE(factor,0.0);
            assertLE(factor,1.0);
            double const targetCountDouble = initialCount * factor;
            size_t const targetCountUnclamped = static_cast<size_t>(std::round(targetCountDouble));
            // we need this as an integer, also we want to get at least one less, but keep at least one
            size_t const targetCount = std::max(std::min(targetCountUnclamped,initialCount-1u),1u);
            assertGE(targetCount,1u);
            assertLT(targetCount,initialCount);

            // we want to divide the number of iterations among each of the initial decks
            double const numberOfIterationsDouble = static_cast<double>(totalNumberOfIterations) / static_cast<double>(initialCount);
            //std::clog << totalNumberOfIterations << "/" << initialCount << "=" << numberOfIterationsDouble;
            unsigned long const numberOfIterationsUnclamped = static_cast<unsigned long>(std::round(numberOfIterationsDouble));
            unsigned long const numberOfIterations = std::max(numberOfIterationsUnclamped, minimalNumberOfIterationsEach);
            assertGE(numberOfIterations,1ul);
            unsigned long const realTotalNumberOfIterations = numberOfIterations * initialCount;

            // first we need to simulate them all
            std::map<DeckTemplate::Ptr, Result> results;            
            std::clog << "\t\tBeginning simulations of ";
            std::clog << std::setw(4) << initialCount << " decks with ";
            std::clog << std::setw(10) << numberOfIterations << " iterations each." << std::endl;
            std::clog << "\t\t";
            {
                unsigned long done = 0;
                unsigned long lastDone = 0;
                unsigned long const dotEverySteps = std::max(realTotalNumberOfIterations / 50, 1ul);
                for(DeckVector::const_iterator iter = decks.begin()
                   ;iter != decks.end()
                   ;iter++)
                {
                    SimulationTaskClass task = withIterationsAndDeck(originalTask
                                                                    ,optimizeAttacker
                                                                    ,*iter
                                                                    ,numberOfIterations
                                                                    );
                    Result result = this->core->simulate(task);
                    assertGE(result.numberOfGames,numberOfIterations);
                    results[*iter] = result;

                    if (this->aborted) {
                        std::cerr << "\tAborted in simpleCrop during simulation stage." << std::endl;
                        return;
                    }
                    done += numberOfIterations;
                    while (done - lastDone > dotEverySteps) {
                        std::clog << ".";
                        lastDone += dotEverySteps;
                    }
                }
            }
            std::clog << "done" << std::endl;

            // next we need to sort them

            // that somehow failes, unclear why.
            //Compare comparator(results);
            //std::sort(decks.begin(), decks.end(), comparator);
            
            {
                unsigned int done = 0;
                unsigned int lastDone = 0;
                unsigned int dotEverySteps = std::max(initialCount / 50, 1u);
                std::clog << "\t\tBeginning sorting" << std::endl << "\t\t";
                for(size_t n = initialCount ; n > 1; ) {
                    size_t newN = 1;
                    for(size_t i = 0; i< n-1; i++) {
                        DeckTemplate::Ptr const a = decks[i];
                        DeckTemplate::Ptr const b = decks[i+1];
                        Result const & ra = results.at(a);
                        Result const & rb = results.at(b);
                        if (ra.getWinRate() < rb.getWinRate()) {
                            decks[i] = b;
                            decks[i+1] = a;
                            newN = i+1;
                        }
                    }
                    done += n-newN;
                    while (done - lastDone > dotEverySteps) {
                        std::clog << ".";
                        lastDone += dotEverySteps;
                    }
                    n = newN;
                }
            }
            std::clog << "done ";

            double oldMin = results.at(decks[initialCount-1]).getWinRate();
            double max = results.at(decks[0]).getWinRate();
            
            decks.resize(targetCount);

            double newMin = results.at(decks[targetCount-1]).getWinRate();
            std::clog << "from [" << oldMin << ";" << max << "] to ";
            std::clog << "[" << newMin << ";" << max << "]";
            std::clog << std::endl;


            assertEQ(decks.size(), targetCount);
       }

                                                
        DeckTemplate::Ptr
        PraetorianOptimizer::optimizeOnce(SimulationTaskClass const & initialTask
                                         ,bool optimizeAttacker
                                         ,double const factor
                                         )
        {
            unsigned long totalNumberOfIterations = initialTask.minimalNumberOfGames;

            std::clog << "\tGenerating mutations... " << std::endl;
            
            DeckTemplate::Ptr initialDeck = optimizeAttacker ? initialTask.attacker : initialTask.defender;
            DeckSet candidateSet = this->mutator->mutate(initialDeck);
            std::vector<DeckTemplate::Ptr> candidateVector(candidateSet.begin(), candidateSet.end());

            std::clog << "\tgot " << candidateVector.size() << " mutations" << std::endl;

            unsigned int cropRound = 0;
            while (candidateVector.size() > 1) {
                std::clog << "\tstarting crop round " << std::setw(6) << cropRound;
                std::clog << " with " << std::setw(6) << candidateVector.size() << " decks";
                std::clog << " and crop factor of " << std::setw(10) << factor << std::endl;
                this->simpleCrop(initialTask
                                ,candidateVector
                                ,factor
                                ,totalNumberOfIterations
                                ,cropRound+1
                                ,optimizeAttacker
                                );                
                
                if (this->aborted) {
                    std::cerr << "\tAborted in optimizeOnce" << std::endl;
                    return candidateVector.front();
                }
                cropRound++;
            }
            assertX(candidateVector.size() == 1);
            return candidateVector.front();
        }

        DeckTemplate::Ptr
        PraetorianOptimizer::optimizeMany(SimulationTaskClass const & task, bool optimizeAttacker)
        {
            double const factor = 0.5;
            // remember old deck
            DeckTemplate::Ptr oldDeckTemplate = optimizeAttacker ? task.attacker : task.defender;
            std::clog << "Beginning optimization of " << oldDeckTemplate->toString() << std::endl;
            unsigned int round = 0;
            unsigned int sameRound = 0;

            while (true) {
                std::clog << "Beginning round " << std::setw(4) << round << " with deck " << oldDeckTemplate->toString() << std::endl;
                // optimize once
                unsigned long minimalNumberOfGames = task.minimalNumberOfGames * (1 + sameRound);
                SimulationTaskClass optimizationTask = withIterationsAndDeck(task, optimizeAttacker, oldDeckTemplate, minimalNumberOfGames);
                DeckTemplate::Ptr newDeckTemplate = this->optimizeOnce(
                    optimizationTask
                   ,optimizeAttacker
                   ,std::pow(factor,1.0 / (1+sameRound))
                );

                if (this->aborted) {
                    std::cerr << "Aborted in optimizeMany during first stage." << std::endl;
                    break;
                }

                std::clog << "Found a (new) deck " << newDeckTemplate->toString() << std::endl;

                // P: Okay, thats a bit hacky, but in practice these will only be IDS or ORDERED_IDS,
                //    which both should have a well-defined string representation.
                if(newDeckTemplate->toString() == oldDeckTemplate->toString()) {
                    break;
                }

                // compare the two
                Result oldResult = this->core->simulate(withDeck(task, optimizeAttacker, oldDeckTemplate));
                Result newResult = this->core->simulate(withDeck(task, optimizeAttacker, newDeckTemplate));

                if (this->aborted) {
                    std::cerr << "Aborted in optimizeMany during second stage." << std::endl;
                    break;
                }

                std::clog << "old deck " << oldDeckTemplate->toString() << " with winrate " << oldResult.getWinRate() << std::endl;
                std::clog << "new deck " << newDeckTemplate->toString() << " with winrate " << newResult.getWinRate() << std::endl;

                if (oldResult.getWinRate() > newResult.getWinRate()) {
                    // new deck is not better :(
                    sameRound++;
                } else {
                    oldDeckTemplate = newDeckTemplate;
                    sameRound = 0;
                }

                round++;
            }
            return oldDeckTemplate;
        }

        void
        PraetorianOptimizer::abort() {
            this->mutator->abort();
            this->aborted = true;
        }

        
    }
}
