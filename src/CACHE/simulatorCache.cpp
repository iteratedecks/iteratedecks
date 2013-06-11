#include "simulatorCache.hpp"

namespace IterateDecks {
    namespace Cache {

        SimulatorCache::SimulatorCache(SimulatorCore::Ptr & delegate)
        : delegate(delegate)
        {
        }

        SimulatorCache::~SimulatorCache()
        {
        }

        IterateDecks::Core::SimulatorCore::Ptr
        SimulatorCache::getDelegate() const {
            return this->delegate;
        }
        
        std::string
        SimulatorCache::getCoreName() const
        {
            return this->delegate->getCoreName();
        }
        
        std::string
        SimulatorCache::getCoreVersion() const
        {
            return this->delegate->getCoreVersion();
        }

    }
}
