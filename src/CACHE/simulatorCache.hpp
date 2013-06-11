#ifndef ITERATEDECKS_CACHE_SIMULATORCACHE_HPP
    #define ITERATEDECKS_CACHE_SIMULATORCACHE_HPP

    #include "../CORE/iterateDecksCore.hpp"

    namespace C = IterateDecks::Core;
    namespace IterateDecks {
        namespace Cache {
    

            class SimulatorCache : public C::SimulatorCore {

                protected:
                    C::SimulatorCore::Ptr delegate;

                public:
                    SimulatorCache(SimulatorCore::Ptr & delegate);
                    virtual ~SimulatorCache();
                    
                    C::SimulatorCore::Ptr getDelegate() const;
                    std::string getCoreName() const;
                    std::string getCoreVersion() const;
                    virtual C::Result simulate(C::SimulationTaskClass const &) = 0;
                    virtual C::Result simulate(C::SimulationTaskClass const &, unsigned long numberOfNewSamples) = 0;
                    virtual void abort() = 0;
            };
        }
    }
#endif
