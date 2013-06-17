#include <iterator>
#include <map>
#include <vector>

#include "cardPool.forward.hpp"
#include "simpleTypes.hpp"

#include "raidDeck.hpp"

#include <set>

#include "activeDeck.hpp"
#include "cardDB.hpp"
#include "cardPool.hpp"
#include "pooledTemplate.hpp"
#include "raidInfo.hpp"

namespace IterateDecks { namespace Core { class Card; } }

namespace IterateDecks {
    namespace Core {

		RaidDeck::RaidDeck(unsigned int raidId)
		: raidId(raidId)
		{
		}

        ActiveDeck
        RaidDeck::instantiate(CardDB const & cardDB) const
        {
			RaidInfo const & raidInfo = cardDB.getRaidInfo(raidId);

			unsigned int const & commanderId(raidInfo.GetCommander());
			Card const * commander = &cardDB.GetCard(commanderId);

			for(VID::const_iterator iter = raidInfo.AlwaysInclude.begin()
			   ;iter != raidInfo.AlwaysInclude.end()
			   ;iter++)
			{
				unsigned int const cardId = *iter;
				Card const * cardPtr = &cardDB.GetCard(cardId);
				alwaysInclude.insert(cardPtr);
			}

			for(VCARDPOOL::const_iterator iter = raidInfo.Pools.begin()
			   ;iter != raidInfo.Pools.end()
			   ;iter++)
			{
				pools.insert(&(*iter));
			}

			ActiveDeck activeDeck(commander, cardDB.GetPointer());
			activeDeck.SetOrderMatters(false);
			//add alwaysinclude
			for(std::multiset<Card const *>::const_iterator iter = alwaysInclude.begin()
			   ;iter != alwaysInclude.end()
			   ;iter++)
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

			//std::clog << activeDeck.GetDeck() << std::endl;
			//activeDeck.PrintShort();
			return activeDeck;

        }
    }
}
