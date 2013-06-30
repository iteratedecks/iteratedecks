#ifndef ITERATEDECKS_CORE_POOLEDDECKTEMPLATE_HPP
    #define ITERATEDECKS_CORE_POOLEDDECKTEMPLATE_HPP

	#include <cstddef>
	#include <set>
	#include <string>

	#include "cardPool.forward.hpp"
	#include "deckTemplate.hpp"
	#include "simpleTypes.hpp"

    namespace IterateDecks {
        namespace Core {

        	class ActiveDeck;
        	class Card;
        	class CardDB;
        	struct CardPool;

        	class PooledTemplate : public DeckTemplate {

                public:
                    typedef std::shared_ptr<PooledTemplate> Ptr;

        		protected:
        			PooledTemplate();
        			Card const * commander;
        			std::multiset<Card const *> alwaysInclude;
        			std::multiset<CardPool const *> pools;

        		public:
        			PooledTemplate(Card const * commander, VCARDPOOL const & pools, CardDB const & cardDB);
        			PooledTemplate(Card const * commander, VID const & alwaysInclude, VCARDPOOL const & pools, CardDB const & cardDB);

        			ActiveDeck instantiate(CardDB const & cardDB) const;

        			DeckTemplate::Ptr withCommander(UINT commanderId) const;
        			size_t getNumberOfNonCommanderCards() const;
        			UINT getCardAtIndex(size_t index) const;
        			DeckTemplate::Ptr withCardAtIndex(unsigned int cardId, size_t index) const;
                    DeckTemplate::Ptr withoutCardAtIndex(size_t index) const;
                    unsigned int getCommander() const;
                    DeckTemplate::Ptr swapCards(size_t i, size_t j) const;

        	};

        }
    }

#endif
