#ifndef ITERATEDECKS_OPT_PRAETORIANOPTIMIZER_HPP
    #define ITERATEDECKS_OPT_PRAETORIANOPTIMIZER_HPP

    #include <set>
    #include "../CORE/deckTemplate.hpp"
    #include "optimizer.hpp"
    #include "mutator.hpp"

    namespace IterateDecks {
        namespace Opt {

            typedef std::vector<IterateDecks::Core::DeckTemplate::Ptr> DeckVector;

            class PraetorianOptimizer : public Optimizer {
                private:
                    IterateDecks::Core::SimulatorCore::Ptr core;
                    Mutator::Ptr mutator;
                    bool aborted;
                    
                private:
                    void simpleCrop(IterateDecks::Core::SimulationTaskClass const & originalTask
                                   ,std::vector<IterateDecks::Core::DeckTemplate::Ptr> & decks
                                   ,double factor
                                   ,unsigned long totalNumberOfIterations
                                   ,unsigned long minimalNumberOfIterationsEach
                                   ,bool optimizeAttacker
                                   );

                public:
                    typedef std::shared_ptr<PraetorianOptimizer> Ptr;

                public:
                    PraetorianOptimizer(IterateDecks::Core::SimulatorCore::Ptr const & simulator, Mutator::Ptr const & mutator);
                
                    virtual IterateDecks::Core::DeckTemplate::Ptr optimizeOnce(IterateDecks::Core::SimulationTaskClass const & initial, bool optimizeAttacker = true, double const cropFactor = 0.5);
                    virtual IterateDecks::Core::DeckTemplate::Ptr optimizeMany(IterateDecks::Core::SimulationTaskClass const & task, bool optimizeAttacker = true);
                    virtual void abort();
                    virtual ~PraetorianOptimizer() {};
            };
        }
    }

#endif

