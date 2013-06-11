#include "simpleOrderedDeckTemplate.hpp"
#include "../CORE/assert.hpp"
#include "../CORE/cardDB.hpp"
#include "../CORE/activeDeck.hpp"
#include <iostream>

using namespace IterateDecks::Core;
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
                this->cards.push_back(cardId);
            }
        }

        ActiveDeck SimpleOrderedDeckTemplate::instantiate(CardDB const & cardDB) const
        {
            Card const & commanderCard = cardDB.GetCard(this->commander);
            ActiveDeck activeDeck(&commanderCard, cardDB.GetPointer());
            activeDeck.SetOrderMatters(true);
            for(std::list<unsigned int>::const_iterator iter = cards.begin(); iter != cards.end(); iter++) {
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
        SimpleOrderedDeckTemplate::toString() const
        {
            std::stringstream ssString;
            ssString << "ORDERED_IDS:";
            ssString << this->commander;
            for(std::list<unsigned int>::const_iterator iter = this->cards.begin()
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
        SimpleOrderedDeckTemplate::withCommander(unsigned int commanderId) const
        {
            SimpleOrderedDeckTemplate::Ptr copy(new SimpleOrderedDeckTemplate(*this));
            copy->commander = commanderId;
            return copy;
        }

        size_t
        SimpleOrderedDeckTemplate::getNumberOfNonCommanderCards() const
        {
            return this->cards.size();
        }

        unsigned int
        SimpleOrderedDeckTemplate::getCardAtIndex(size_t index) const
        {
            std::list<unsigned int>::const_iterator iter = this->cards.begin();
            for(size_t i = 0u; i < index; i++) {
                iter++;
            }
            return *iter;
        }

        DeckTemplate::Ptr
        SimpleOrderedDeckTemplate::withCardAtIndex(unsigned int cardId, size_t index) const
        {
            SimpleOrderedDeckTemplate::Ptr copy(new SimpleOrderedDeckTemplate(*this));
            std::list<unsigned int>::iterator iter = copy->cards.begin();
            for(size_t i = 0u; i < index; i++) {
                iter++;
            }
            copy->cards.insert(iter,cardId);
            return copy;
        }

        DeckTemplate::Ptr
        SimpleOrderedDeckTemplate::withoutCardAtIndex(size_t index) const
        {
            SimpleOrderedDeckTemplate::Ptr copy(new SimpleOrderedDeckTemplate(*this));
            std::list<unsigned int>::iterator iter = copy->cards.begin();
            for(size_t i = 0u; i < index; i++) {
                iter++;
            }
            copy->cards.erase(iter);
            return copy;
        }

        unsigned int
        SimpleOrderedDeckTemplate::getCommander() const
        {
            return this->commander;
        }

        DeckTemplate::Ptr
        SimpleOrderedDeckTemplate::swapCards(size_t i
                                            ,size_t j
                                            ) const
        {
            SimpleOrderedDeckTemplate::Ptr copy(new SimpleOrderedDeckTemplate(*this));
            std::list<unsigned int>::iterator iterI = copy->cards.begin();
            for(size_t ii = 0u; ii < i; ii++) {
                iterI++;
            }
            std::list<unsigned int>::iterator iterJ = copy->cards.begin();
            for(size_t jj = 0u; jj < j; jj++) {
                iterJ++;
            }
            unsigned int cardAtI = *iterI;
            unsigned int cardAtJ = *iterJ;
            *iterI = cardAtJ;
            *iterJ = cardAtI;
            return copy;
        }

        bool
        operator< (SimpleOrderedDeckTemplate const & a
                  ,SimpleOrderedDeckTemplate const & b
                  )
        {
            if (a.commander < b.commander) {
                return true;
            } else if (a.commander > b.commander) {
                return false;
            } else {
                return a.cards < b.cards;
            }
        }

    }
}
