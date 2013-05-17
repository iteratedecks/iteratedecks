#ifndef ITERATEDECKS_CACHE_SIMULATORCACHE_HPP
    #define ITERATEDECKS_CACHE_SIMULATORCACHE_HPP

    #include "../CORE/iterateDecksCore.hpp"

    namespace IterateDecks {
        namespace Cache {
    

            class SimulatorCache : public SimulatorCore {

                protected:
                    SimulatorCore::Ptr delegate;

                public:
                    SimulatorCache(SimulatorCore::Ptr & delegate);
                    virtual ~SimulatorCache();
                    
                    SimulatorCore::Ptr getDelegate() const;
                    std::string getCoreName() const;
                    std::string getCoreVersion() const;
                    virtual IterateDecks::Core::Result simulate(SimulationTaskClass const &) = 0;
                    virtual IterateDecks::Core::Result simulate(SimulationTaskClass const &, unsigned long numberOfNewSamples) = 0;
                    virtual void abort() = 0;
            };
        }
    }
#endif
