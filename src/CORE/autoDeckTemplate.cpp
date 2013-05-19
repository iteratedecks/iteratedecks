#include "autoDeckTemplate.hpp"
#include "assert.hpp"
#include "cardDB.hpp"
#include "activeDeck.hpp"
#include <iostream>

namespace IterateDecks {
    namespace Core {

        AutoDeckTemplate::AutoDeckTemplate(std::list<unsigned int> const & ids)
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

        ActiveDeck AutoDeckTemplate::instantiate(CardDB const & cardDB) const
        {
            Card const & commanderCard = cardDB.GetCard(this->commander);
            ActiveDeck activeDeck(&commanderCard, cardDB.GetPointer());
            activeDeck.SetOrderMatters(false);
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

        std::string
        AutoDeckTemplate::toString() const
        {
            std::stringstream ssString;
            ssString << "IDS:";
            ssString << this->commander;
            for(std::multiset<unsigned int>::const_iterator iter = this->cards.begin()
               ;iter != this->cards.end()
               ;iter++
               )
            {
                ssString << ",";
                ssString << *iter;
            }
            return ssString.str();
        }

        DeckTemplate::Ptr
        AutoDeckTemplate::withCommander(unsigned int commanderId) const
        {
            AutoDeckTemplate::Ptr copy(new AutoDeckTemplate(*this));
            copy->commander = commanderId;
            return copy;
        }

        size_t
        AutoDeckTemplate::getNumberOfNonCommanderCards() const
        {
            return this->cards.size();
        }

        unsigned int
        AutoDeckTemplate::getCardAtIndex(size_t index) const
        {
            std::multiset<unsigned int>::const_iterator iter = this->cards.begin();
            for(size_t i = 0u; i < index; i++) {
                iter++;
            }
            return *iter;
        }

        DeckTemplate::Ptr
        AutoDeckTemplate::withCardAtIndex(unsigned int cardId, size_t index) const
        {
            AutoDeckTemplate::Ptr copy(new AutoDeckTemplate(*this));
            copy->cards.insert(cardId);
            return copy;
        }

        DeckTemplate::Ptr
        AutoDeckTemplate::withoutCardAtIndex(size_t index) const
        {
            AutoDeckTemplate::Ptr copy(new AutoDeckTemplate(*this));
            std::multiset<unsigned int>::iterator iter = copy->cards.begin();
            for(size_t i = 0u; i < index; i++) {
                iter++;
            }
            copy->cards.erase(iter);
            return copy;
        }

        unsigned int
        AutoDeckTemplate::getCommander() const
        {
            return this->commander;
        }

        DeckTemplate::Ptr
        AutoDeckTemplate::swapCards(size_t i, size_t j) const
        {
            return DeckTemplate::Ptr(new AutoDeckTemplate(*this));
        }

    }
}
