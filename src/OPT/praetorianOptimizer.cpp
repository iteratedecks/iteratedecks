#include "praetorianOptimizer.hpp"
#include "../CORE/assert.hpp"

namespace IterateDecks {
    namespace Opt {

        PraetorianOptimizer::PraetorianOptimizer(SimulatorCore::Ptr const & simulator
                                                ,Mutator::Ptr const & mutator
                                                )
        : core(simulator)
        , mutator(mutator)
        {
        }

        void
        PraetorianOptimizer::simpleCrop(SimulationTaskClass const & originalTask
                                       ,std::vector<DeckTemplate::Ptr> & decks
                                       ,double factor
                                       ,unsigned int totalNumberOfIterations
                                       )
       {
       }

                                                
        DeckTemplate::Ptr
        PraetorianOptimizer::optimizeOnce(SimulationTaskClass const & initialTask
                                         ,bool optimizeAttacker
                                         )
        {
            double const factor = 0.9;
            unsigned long totalNumberOfIterations = initialTask.minimalNumberOfGames;
            
            DeckTemplate::Ptr initialDeck = optimizeAttacker ? initialTask.attacker : initialTask.defender;
            std::set<DeckTemplate::Ptr> candidateSet = this->mutator->mutate(initialDeck);
            std::vector<DeckTemplate::Ptr> candidateVector(candidateSet.begin(), candidateSet.end());
            
            while (candidateVector.size() > 1) {
                this->simpleCrop(initialTask
                                ,candidateVector
                                ,factor
                                ,totalNumberOfIterations
                                );
            }
            assertX(candidateVector.size() == 1);
            return *(candidateVector.begin());
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
        
        
        DeckTemplate::Ptr
        PraetorianOptimizer::optimizeMany(SimulationTaskClass const & task, bool optimizeAttacker)
        {
            // remember old deck
            DeckTemplate::Ptr oldDeckTemplate = optimizeAttacker ? task.attacker : task.defender;

            while (true) {
                // optimize once
                SimulationTaskClass optimizationTask = withDeck(task, optimizeAttacker, oldDeckTemplate);
                DeckTemplate::Ptr newDeckTemplate = this->optimizeOnce(optimizationTask, optimizeAttacker);

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
            }
            return oldDeckTemplate;
        }
        
    }
}
