#include "questDeck.hpp"

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
#include "simpleTypes.hpp"
#include "stepInfo.hpp"

namespace IterateDecks {
    namespace Core {

        QuestDeck::QuestDeck(unsigned int questId, Core::CardDB const & cardDB)
        : questId(questId)
        {
            StepInfo const & stepInfo = cardDB.getQuestInfo(questId);

            unsigned int const & commanderId(stepInfo.GetCommander());
            this->commander = &cardDB.GetCard(commanderId);

            for(VCARDPOOL::const_iterator iter = stepInfo.pools.begin()
               ;iter != stepInfo.pools.end()
               ;iter++)
            {
                pools.insert(&(*iter));
            }
        }

        ActiveDeck
        QuestDeck::instantiate(CardDB const & cardDB) const
        {
            ActiveDeck activeDeck(commander, cardDB.GetPointer());
            activeDeck.SetOrderMatters(false);

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

        std::string QuestDeck::toString() const
        {
            return "QUESTID:" + boost::lexical_cast<std::string>(this->questId);
        }
    }
}
