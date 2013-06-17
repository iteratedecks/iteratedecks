#include "pooledTemplate.hpp"

#include <bits/shared_ptr.h>
#include <iterator>
#include <set>
#include <vector>

#include "activeDeck.hpp"
#include "assert.hpp"
#include "cardDB.hpp"
#include "cardPool.forward.hpp"
#include "cardPool.hpp"
#include "constants.hpp"
#include "deckTemplate.hpp"
#include "raidInfo.hpp"
#include "simpleTypes.hpp"

namespace IterateDecks {
    namespace Core {

    	PooledTemplate::PooledTemplate()
    	: commander()
    	{
    	}

		PooledTemplate::PooledTemplate(Card const * commander
									  ,VCARDPOOL const & pools
									  ,CardDB const & cardDB
									  )
		: DeckTemplate()
		, commander(commander)
		{
			for(VCARDPOOL::const_iterator iter = pools.begin()
			   ;iter != pools.end()
			   ;iter++)
			{
				this->pools.insert(&(*iter));
			}
		}

		PooledTemplate::PooledTemplate(Card const * commander
									  ,VID const & alwaysInclude
									  ,VCARDPOOL const & pools
									  ,CardDB const & cardDB
									  )
		: DeckTemplate()
		, commander(commander)
		{

			for(VID::const_iterator iter = alwaysInclude.begin()
			   ;iter != alwaysInclude.end()
			   ;iter++)
			{
				unsigned int const cardId = *iter;
				Card const * cardPtr = &cardDB.GetCard(cardId);
				this->alwaysInclude.insert(cardPtr);
			}

			for(VCARDPOOL::const_iterator iter = pools.begin()
			   ;iter != pools.end()
			   ;iter++)
			{
				this->pools.insert(&(*iter));
			}
		}

		DeckTemplate::Ptr PooledTemplate::createFromRaidId(unsigned int const raidId, CardDB const & cardDB)
		{
			assertLT(raidId,RAID_MAX_ID);
			RaidInfo const & raidInfo = cardDB.getRaidInfo(raidId);
			assertX(raidInfo.IsValid());
			unsigned int const & commanderId(raidInfo.GetCommander());
			Card const * commander = &cardDB.GetCard(commanderId);

			return DeckTemplate::Ptr(new PooledTemplate(commander, raidInfo.AlwaysInclude, raidInfo.Pools, cardDB));
		}

        DeckTemplate::Ptr PooledTemplate::createFromQuestId(unsigned int const questId, CardDB const & cardDB)
        {
            assertLT(questId,STEP_MAX_ID);
            StepInfo const & stepInfo = cardDB.getQuestInfo(questId);
            assertX(stepInfo.IsValid());
            unsigned int const & commanderId(stepInfo.GetCommander());
            Card const * commander = &cardDB.GetCard(commanderId);

            return Core::DeckTemplate::Ptr(new PooledTemplate(commander, stepInfo.pools, cardDB));
        }

		ActiveDeck PooledTemplate::instantiate(CardDB const & cardDB) const
		{
			ActiveDeck activeDeck(this->commander, cardDB.GetPointer());
			activeDeck.SetOrderMatters(false);
			//add alwaysinclude
			for(std::multiset<Card const *>::const_iterator iter = alwaysInclude.begin()
			   ;iter != alwaysInclude.end()
			   ; iter++)
			{
				activeDeck.Add(*iter);
			}
			// pools
			for(std::multiset<CardPool const *>::const_iterator iter = pools.begin()
			   ;iter != pools.end()
			   ;iter++)
			{
				CardPool const * pool = *iter;
				pool->GetPool(cardDB.GetPointer(), activeDeck.Deck);
			}

			assertX(activeDeck.IsValid());
			//std::clog << activeDeck.GetDeck() << std::endl;
			//activeDeck.PrintShort();
			return activeDeck;

		}

		std::string PooledTemplate::toString() const
		{
			throw Exception("Not Implemented.");
		}

		DeckTemplate::Ptr PooledTemplate::withCommander(UINT commanderId) const
		{
			throw Exception("Not Implemented.");
		}

		size_t PooledTemplate::getNumberOfNonCommanderCards() const
		{
			throw Exception("Not Implemented.");
		}

		UINT PooledTemplate::getCardAtIndex(size_t index) const
		{
			throw Exception("Not Implemented.");
		}

		DeckTemplate::Ptr PooledTemplate::withCardAtIndex(unsigned int cardId, size_t index) const
		{
			throw Exception("Not Implemented.");
		}

		DeckTemplate::Ptr PooledTemplate::withoutCardAtIndex(size_t index) const
		{
			throw Exception("Not Implemented.");
		}

		unsigned int PooledTemplate::getCommander() const
		{
			throw Exception("Not Implemented.");
		}

		DeckTemplate::Ptr PooledTemplate::swapCards(size_t i, size_t j) const
		{
			throw Exception("Not Implemented.");
		}

    }
}
