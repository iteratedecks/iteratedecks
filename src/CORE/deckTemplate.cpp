#include "deckTemplate.hpp"
//#include "activeDeck.hpp"
//#include "cardDB.hpp"
//#include "assert.hpp"
//#include "cardPool.hpp"

//#include <iostream>

namespace IterateDecks {
    namespace Core {

        DeckTemplate::DeckTemplate()
        : allowInvalid(false)
        {}

        DeckTemplate::~DeckTemplate() {}

        DeckTemplate::Ptr
        DeckTemplate::withCard(unsigned int cardId) const
        {
            return this->withCardAtIndex(cardId, 0u);
        }

        DeckTemplate::Ptr
        DeckTemplate::replaceCardAtIndex(unsigned int cardId
                                        ,size_t index
                                        ) const
        {
            return this->withoutCardAtIndex(index)->withCardAtIndex(cardId, index);
        }
        
    }
}
