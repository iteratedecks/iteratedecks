#ifndef ITERATEDECKS_CACHE_DISKBACKEDCACHE_HPP
    #define ITERATEDECKS_CACHE_DISKBACKEDCACHE_HPP

    #include "simulatorCache.hpp"
    #include "../CORE/iterateDecksCore.hpp"
    #include "sqliteWrapper.hpp"
    #include <cstdlib>

    namespace C = IterateDecks::Core;
    namespace IterateDecks {
        namespace Cache {    

            class DiskBackedCache : public SimulatorCache {
                private:
                    bool ignoreCoreVersion;
                    bool ignoreXMLVersion;
                    bool dontReadFromCache;
                    SQLiteWrapper database;
                    
                    PreparedStatement * insertStatement;
                    PreparedStatement * selectStatement;

                    unsigned int randomData;
                
                    C::Result loadCache(C::SimulationTaskClass const & task);
                    void addToCache(C::SimulationTaskClass const & task, C::Result const & result);
                public:
                    typedef std::shared_ptr<DiskBackedCache> Ptr;
                
                public:
                    DiskBackedCache(SimulatorCore::Ptr & delegate);
                    virtual ~DiskBackedCache();
                    void setDontReadCache(bool);

                    virtual C::Result simulate(C::SimulationTaskClass const &);
                    virtual C::Result simulate(C::SimulationTaskClass const &, unsigned long numberOfNewSamples);
                    virtual void abort();                    
            };
        }
    }

#endif

