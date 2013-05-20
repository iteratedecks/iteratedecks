#include "praetorianOptimizer.hpp"
#include "../CORE/assert.hpp"
#include <iostream>
#include <iomanip>

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

        void
        PraetorianOptimizer::simpleCrop(SimulationTaskClass const & originalTask
                                       ,std::vector<DeckTemplate::Ptr> & decks
                                       ,double factor
                                       ,unsigned long totalNumberOfIterations
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
           double const numberOfIterationsDouble = totalNumberOfIterations / factor;
           unsigned long const numberOfIterations = static_cast<unsigned long>(std::round(numberOfIterationsDouble));
           assertGE(numberOfIterations,1ul);
           unsigned long const realTotalNumberOfIterations = numberOfIterations * initialCount;
           unsigned long done = 0;
           unsigned long lastDone = 0;
           unsigned long const dotEverySteps = realTotalNumberOfIterations / 50;
           
           // first we need to simulate them all
           std::clog << "\t\tBeginning simulations of " << std::setw(4) << initialCount << " decks with " << std::setw(10) << numberOfIterations << " iterations each." << std::endl;
           std::clog << "\t\t";
           for(DeckVector::const_iterator iter = decks.begin()
              ;iter != decks.end()
              ;iter++)
            {
                SimulationTaskClass task = withIterationsAndDeck(originalTask
                                                               ,optimizeAttacker
                                                               , *iter
                                                               , numberOfIterations
                                                               );
                this->core->simulate(task);
                
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
           std::clog << "done" << std::endl;
           throw Exception("Not implemented!");
       }

                                                
        DeckTemplate::Ptr
        PraetorianOptimizer::optimizeOnce(SimulationTaskClass const & initialTask
                                         ,bool optimizeAttacker
                                         )
        {
            double const factor = 0.9;
            unsigned long totalNumberOfIterations = initialTask.minimalNumberOfGames;

            std::clog << "Generating mutations... ";
            
            DeckTemplate::Ptr initialDeck = optimizeAttacker ? initialTask.attacker : initialTask.defender;
            std::set<DeckTemplate::Ptr> candidateSet = this->mutator->mutate(initialDeck);
            std::vector<DeckTemplate::Ptr> candidateVector(candidateSet.begin(), candidateSet.end());

            std::clog << "got " << candidateVector.size() << " mutations" << std::endl;

            unsigned int cropRound = 0;
            while (candidateVector.size() > 1) {
                std::clog << "\tstarting crop round " << std::setw(6) << cropRound << " with " << std::setw(6) << candidateVector.size() << " decks" << std::endl;
                this->simpleCrop(initialTask
                                ,candidateVector
                                ,factor
                                ,totalNumberOfIterations
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
            // remember old deck
            DeckTemplate::Ptr oldDeckTemplate = optimizeAttacker ? task.attacker : task.defender;
            std::clog << "Beginning optimization of " << oldDeckTemplate->toString() << std::endl;
            unsigned int round = 0;

            while (true) {
                std::clog << "Beginning round " << std::setw(4) << round << " with deck " << oldDeckTemplate->toString() << std::endl;
                // optimize once
                SimulationTaskClass optimizationTask = withDeck(task, optimizeAttacker, oldDeckTemplate);
                DeckTemplate::Ptr newDeckTemplate = this->optimizeOnce(optimizationTask, optimizeAttacker);

                std::clog << "Found a (new) deck " << newDeckTemplate->toString() << std::endl;

                // P: Okay, thats a bit hacky, but in practice these will only be IDS or ORDERED_IDS,
                //    which both should have a well-defined string representation.
                if(newDeckTemplate->toString() == oldDeckTemplate->toString()) {
                    break;
                }

                // compare the two
                Result oldResult = this->core->simulate(withDeck(task, optimizeAttacker, oldDeckTemplate));
                Result newResult = this->core->simulate(withDeck(task, optimizeAttacker, newDeckTemplate));

                if (oldResult.getWinRate() > newResult.getWinRate()) {
                    // new deck is not better :(
                } else {
                    oldDeckTemplate = newDeckTemplate;
                }

                if (this->aborted) {
                    std::cerr << "\tAborted in optimizeMany" << std::endl;
                    break;
                }
                round++;
            }
            return oldDeckTemplate;
        }

        void
        PraetorianOptimizer::abort() {
            this->aborted = true;
        }

        
    }
}
