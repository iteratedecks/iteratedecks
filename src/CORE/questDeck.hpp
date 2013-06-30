#ifndef ITERATEDECKS_CORE_QUESTDECK_HPP
    #define ITERATEDECKS_CORE_QUESTDECK_HPP

#include <memory>
#include "cardDB.hpp"
#include "pooledTemplate.hpp"

namespace IterateDecks {
    namespace Core {

        class QuestDeck : public PooledTemplate {
        public:
            typedef std::shared_ptr<QuestDeck> Ptr;

        private:
            unsigned int questId;

        public:
            QuestDeck(unsigned int questId, Core::CardDB const & cardDB);
            ActiveDeck instantiate(IterateDecks::Core::CardDB const &) const;
            std::string toString() const;
        };
    }
}


#endif
