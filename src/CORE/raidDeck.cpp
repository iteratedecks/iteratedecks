#include "raidDeck.hpp"

#include <boost/lexical_cast.hpp>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "activeDeck.hpp"
#include "card.hpp"
#include "cardDB.hpp"
#include "cardPool.forward.hpp"
#include "cardPool.hpp"
#include "pooledTemplate.hpp"
#include "raidInfo.hpp"
#include "simpleTypes.hpp"

namespace IterateDecks {
    namespace Core {

        RaidDeck::RaidDeck(unsigned int raidId, Core::CardDB const & cardDB)
        : raidId(raidId)
        {
            RaidInfo const & raidInfo = cardDB.getRaidInfo(raidId);

            unsigned int const & commanderId(raidInfo.GetCommander());
            this->commander = &cardDB.GetCard(commanderId);

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
        }

        ActiveDeck
        RaidDeck::instantiate(CardDB const & cardDB) const
        {
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

        std::string RaidDeck::toString() const
        {
            return "RAIDID:" + boost::lexical_cast<std::string>(this->raidId);
        }
    }
}
