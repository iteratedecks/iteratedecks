#include "multiDeckTemplate.hpp"
#include "assert.hpp"
#include "cardDB.hpp"
#include "activeDeck.hpp"
#include <iostream>
#include <boost/foreach.hpp>

namespace IterateDecks {
    namespace Core {

        MultiDeckTemplate::MultiDeckTemplate(
            std::multiset<DeckTemplate::Ptr> const & decks
            )
        : decks(decks)
        {
        }

        ActiveDeck MultiDeckTemplate::instantiate(CardDB const & cardDB) const
        {
            // select a random element
            unsigned int const size = this->decks.size();
            // not uniform distributed, however error is small enough if size << MAX_INT;
            // which we can assume here
            unsigned int const index = rand() % size;
            std::multiset<DeckTemplate::Ptr>::const_iterator iter = this->decks.begin();
            std::advance(iter, index);
            // instantiate that one
            return (*iter)->instantiate(cardDB);
        }

        std::string MultiDeckTemplate::toString() const
        {
            std::stringstream ssString;
            BOOST_FOREACH(DeckTemplate::Ptr const & deck, this->decks) {
                ssString << '{';
                ssString << deck->toString();
                ssString << '}';
                ssString << ';';
            }
            return ssString.str();
        }

        DeckTemplate::Ptr MultiDeckTemplate::withCommander(UINT commanderId) const
		{
			throw Exception("Not Implemented.");
		}

		size_t MultiDeckTemplate::getNumberOfNonCommanderCards() const
		{
			throw Exception("Not Implemented.");
		}

		UINT MultiDeckTemplate::getCardAtIndex(size_t index) const
		{
			throw Exception("Not Implemented.");
		}

		DeckTemplate::Ptr MultiDeckTemplate::withCardAtIndex(unsigned int cardId, size_t index) const
		{
			throw Exception("Not Implemented.");
		}

		DeckTemplate::Ptr MultiDeckTemplate::withoutCardAtIndex(size_t index) const
		{
			throw Exception("Not Implemented.");
		}

		unsigned int MultiDeckTemplate::getCommander() const
		{
			throw Exception("Not Implemented.");
		}

		DeckTemplate::Ptr MultiDeckTemplate::swapCards(size_t i, size_t j) const
		{
			throw Exception("Not Implemented.");
		}

        std::multiset<DeckTemplate::Ptr> const &
        MultiDeckTemplate::getDecks() const
        {
            return this->decks;
        }

    }
}
