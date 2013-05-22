#include "deckTemplate.hpp"
#include "cardDB.hpp"
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

        bool
        DeckTemplate::isValid(CardDB const & cardDB) const
        {
            unsigned int const commanderId = this->getCommander();
            Card const & commanderCard = cardDB.GetCard(commanderId);
            if(commanderCard.GetType() != TYPE_COMMANDER) {
                return false;
            }

            bool hasLegendary = false;
            std::set<unsigned int> uniques;
            
            for (size_t i = 0; i < this->getNumberOfNonCommanderCards(); i++) {
                unsigned int const cardId = this->getCardAtIndex(i);
                Card const & card = cardDB.GetCard(cardId);
                UINT const rarity = card.GetRarity();
                if (rarity == RARITY_LEGENDARY) {
                    if (hasLegendary) {
                        return false;
                    } else {
                        hasLegendary = true;
                    }
                } else if (rarity == RARITY_UNIQUE) {
                    if (uniques.find(cardId) != uniques.end()) {
                        return false;
                    } else {
                        uniques.insert(cardId);
                    }
                }
            }
            return true;
        }
        
    }
}
