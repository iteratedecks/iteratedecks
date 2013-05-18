#ifndef ITERATEDECKS_OPT_PRAETORIANOPTIMIZER_HPP
    #define ITERATEDECKS_OPT_PRAETORIANOPTIMIZER_HPP

    #include <set>
    #include "../CORE/deckTemplate.hpp"
    #include "optimizer.hpp"
    #include "mutator.hpp"

    namespace IterateDecks {
        namespace Opt {

            class PraetorianOptimizer : public Optimizer {
                private:
                    SimulatorCore::Ptr core;
                    Mutator::Ptr mutator;
                    
                private:
                    void simpleCrop(SimulationTaskClass const & originalTask, std::vector<DeckTemplate::Ptr> & decks, double factor, unsigned int totalNumberOfIterations);

                public:
                    typedef std::shared_ptr<PraetorianOptimizer> Ptr;

                public:
                    PraetorianOptimizer(SimulatorCore::Ptr const & simulator, Mutator::Ptr const & mutator);
                
                    virtual DeckTemplate::Ptr optimizeOnce(SimulationTaskClass const & initial, bool optimizeAttacker = true);
                    virtual DeckTemplate::Ptr optimizeMany(SimulationTaskClass const & task, bool optimizeAttacker = true);
                    
            };
        }
    }

#endif

