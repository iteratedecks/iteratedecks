#ifndef ITERATEDECKS_OPT_PRAETORIANOPTIMIZER_HPP
    #define ITERATEDECKS_OPT_PRAETORIANOPTIMIZER_HPP

    #include <set>
    #include "../CORE/deckTemplate.hpp"
    #include "optimizer.hpp"
    #include "mutator.hpp"

    namespace IterateDecks {
        namespace Opt {

            typedef std::vector<DeckTemplate::Ptr> DeckVector;

            class PraetorianOptimizer : public Optimizer {
                private:
                    SimulatorCore::Ptr core;
                    Mutator::Ptr mutator;
                    bool aborted;
                    
                private:
                    void simpleCrop(SimulationTaskClass const & originalTask
                                   ,std::vector<DeckTemplate::Ptr> & decks
                                   ,double factor
                                   ,unsigned long totalNumberOfIterations
                                   ,unsigned long minimalNumberOfIterationsEach
                                   ,bool optimizeAttacker
                                   );

                public:
                    typedef std::shared_ptr<PraetorianOptimizer> Ptr;

                public:
                    PraetorianOptimizer(SimulatorCore::Ptr const & simulator, Mutator::Ptr const & mutator);
                
                    virtual DeckTemplate::Ptr optimizeOnce(SimulationTaskClass const & initial, bool optimizeAttacker = true);
                    virtual DeckTemplate::Ptr optimizeMany(SimulationTaskClass const & task, bool optimizeAttacker = true);
                    virtual void abort();
            };
        }
    }

#endif

