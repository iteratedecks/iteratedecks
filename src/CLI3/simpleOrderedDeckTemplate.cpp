#include "simpleOrderedDeckTemplate.hpp"
#include "../CORE/assert.hpp"
#include "../CORE/cardDB.hpp"
#include "../CORE/activeDeck.hpp"
#include <iostream>

namespace IterateDecks {
    namespace CLI3 {

        SimpleOrderedDeckTemplate::SimpleOrderedDeckTemplate(std::list<unsigned int> const & ids)
        : DeckTemplate()
        {
            std::list<unsigned int>::const_iterator iter = ids.begin();
            unsigned int const commanderId = *iter;
            this->commander = commanderId;
            for(iter++ ;iter != ids.end(); iter++) {
                unsigned int cardId = *iter;
                this->cards.insert(cardId);
            }
        }

        ActiveDeck SimpleOrderedDeckTemplate::instantiate(CardDB const & cardDB) const
        {
            Card const & commanderCard = cardDB.GetCard(this->commander);
            ActiveDeck activeDeck(&commanderCard, cardDB.GetPointer());
            activeDeck.SetOrderMatters(true);
            for(std::multiset<unsigned int>::const_iterator iter = cards.begin(); iter != cards.end(); iter++) {
                Card const & card = cardDB.GetCard(*iter);
                activeDeck.Add(&card);
            }
            assertX(activeDeck.Commander.GetOriginalCard() != NULL);
            assertEQ(activeDeck.Commander.GetOriginalCard()->GetType(), TYPE_COMMANDER);
            assertEQ(activeDeck.Commander.GetType(),TYPE_COMMANDER);
            assertX(activeDeck.Commander.IsDefined());
            assertX(this->allowInvalid || activeDeck.IsValid());
            assertX(activeDeck.isAlive());
            return activeDeck;
        }
        
    }
}
