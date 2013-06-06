#ifndef ITERATEDECKS_CACHE_DISKBACKEDCACHE_HPP
    #define ITERATEDECKS_CACHE_DISKBACKEDCACHE_HPP

    #include "simulatorCache.hpp"
    #include "../CORE/iterateDecksCore.hpp"
    #include "sqliteWrapper.hpp"
    #include <cstdlib>

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
                
                    Result loadCache(SimulationTaskClass const & task);
                    void addToCache(SimulationTaskClass const & task, Result const & result);
                public:
                    typedef std::shared_ptr<DiskBackedCache> Ptr;
                
                public:
                    DiskBackedCache(SimulatorCore::Ptr & delegate);
                    virtual ~DiskBackedCache();
                    void setDontReadCache(bool);

                    virtual Result simulate(SimulationTaskClass const &);
                    virtual Result simulate(SimulationTaskClass const &, unsigned long numberOfNewSamples);
                    virtual void abort();                    
            };
        }
    }

#endif

