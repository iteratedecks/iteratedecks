#ifndef ITERATEDECKS_CORE_RAIDDECK_HPP
    #define ITERATEDECKS_CORE_RAIDDECK_HPP

#include <memory>
#include "cardDB.hpp"
#include "pooledTemplate.hpp"

namespace IterateDecks {
    namespace Core {

        class RaidDeck : public PooledTemplate {
        public:
            typedef std::shared_ptr<RaidDeck> Ptr;

        private:
            unsigned int raidId;

        public:
            RaidDeck(unsigned int missionId, CardDB const & cardDB);
            ActiveDeck instantiate(IterateDecks::Core::CardDB const &) const;
        };
    }
}


#endif
