#ifndef ITERATEDECKS_CACHE_MULTIDECKDECOMPOSITOR_HPP
    #define ITERATEDECKS_CACHE_MULTIDECKDECOMPOSITOR_HPP

    #include "../CORE/iterateDecksCore.hpp"

    namespace C = IterateDecks::Core;
    namespace IterateDecks {
        namespace Cache {
    

            class MultiDeckDecompositor : public C::SimulatorCore {

                protected:
                    C::SimulatorCore::Ptr delegate;

                public:
                    MultiDeckDecompositor(SimulatorCore::Ptr delegate);
                    virtual ~MultiDeckDecompositor();
                    
                    C::SimulatorCore::Ptr getDelegate() const;
                    std::string getCoreName() const;
                    std::string getCoreVersion() const;
                    std::string getCoreVersionHumanReadable() const;
                    
                    virtual C::Result simulate(C::SimulationTaskClass const &);
                    virtual void abort();
            };
        }
    }
#endif

