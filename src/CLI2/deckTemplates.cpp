#include "deckTemplates.hpp"
#include <list>
#include "../CORE/assert.hpp"
#include "../CORE/cardDB.hpp"
#include "../CORE/cardPool.hpp"
#include "../CORE/activeDeck.hpp"

namespace IterateDecks {
    namespace CLI {

        std::list<unsigned int> hashToId(std::string const & hash)
        {
            std::list<unsigned int> list;

            size_t const len = hash.length();
            if(len % 2 != 0) {
                throw InvalidDeckHashError(InvalidDeckHashError::notEvenChars);
            }
            assertX(!(len & 1)); // bytes should go in pairs
            size_t const numberOfCards = len / 2;

            unsigned int lastid;
            for (unsigned int i = 0; i < numberOfCards; i++) {
                if (hash[i << 1] == '.') break; // delimeter
                if (isspace(hash[i << 1])) break; // not a hash
                unsigned int tid = base64ToId((hash[i << 1] << 8) + hash[(i << 1) + 1]);
                if (i==0) {
                    // first card is commander
                    assertX(tid < CARD_MAX_ID);
                    assertX((tid >= 1000) && (tid < 2000)); // commander Id boundaries
                    list.push_back(tid);
                } else {
                    // later cards are not commander
                    assertX(i>0 || (tid < CARD_MAX_ID)); // commander card can't be encoded with RLE
                    if (tid < CARD_MAX_ID) {
                        // this is a card
                        list.push_back(tid);
                        lastid = tid;
                    } else {
                        // this is an encoding for rle
                        for (unsigned int k = CARD_MAX_ID+1; k < tid; k++) { // decode RLE, +1 because we already added one card
                            list.push_back(lastid);
                        }
                    }
                }
            }
            return list;
        }

        OrderedDeckTemplate::OrderedDeckTemplate(DeckArgument const & argument, CardDB const & cardDB)
        : DeckTemplate(cardDB)
        {
            assertX(argument.isOrdered());
            std::list<unsigned int> ids = hashToId(argument.getHash());
            std::list<unsigned int>::const_iterator iter = ids.begin();
            unsigned int const commanderId = *iter;
            this->commander = &cardDB.GetCard(commanderId);
            for(iter++ ;iter != ids.end(); iter++) {
                Card const & card = cardDB.GetCard(*iter);
                this->cards.push_back(&card);
            }
        }

        ActiveDeck OrderedDeckTemplate::instantiate() const
        {
            ActiveDeck activeDeck(this->commander, this->cardDB.GetPointer());
            activeDeck.SetOrderMatters(true);
            for(std::list<Card const *>::const_iterator iter = cards.begin(); iter != cards.end(); iter++) {
                activeDeck.Add(*iter);
            }
            assertX(activeDeck.IsValid());
            assertX(activeDeck.isAlive());
            return activeDeck;
        }

        UnorderedDeckTemplate::UnorderedDeckTemplate(DeckArgument const & argument, CardDB const & cardDB)
        : DeckTemplate(cardDB)
        {
            assertX(!argument.isOrdered());
            std::list<unsigned int> ids = hashToId(argument.getHash());
            std::list<unsigned int>::const_iterator iter = ids.begin();
            unsigned int const commanderId = *iter;
            Card const * const commanderCard = &cardDB.GetCard(commanderId);
            assertEQ(commanderCard->GetType(), TYPE_COMMANDER);
            this->commander = commanderCard;
            for(iter++ ;iter != ids.end(); iter++) {
                Card const & card = cardDB.GetCard(*iter);
                this->cards.insert(&card);
            }
        }

        ActiveDeck UnorderedDeckTemplate::instantiate() const
        {
            ActiveDeck activeDeck(this->commander, this->cardDB.GetPointer());
            activeDeck.SetOrderMatters(false);
            for(std::multiset<Card const *>::const_iterator iter = cards.begin(); iter != cards.end(); iter++) {
                activeDeck.Add(*iter);
            }
            assertX(activeDeck.Commander.GetOriginalCard() != NULL);
            assertEQ(activeDeck.Commander.GetOriginalCard()->GetType(), TYPE_COMMANDER);
            assertEQ(activeDeck.Commander.GetType(),TYPE_COMMANDER);
            assertX(activeDeck.Commander.IsDefined());
            assertX(this->allowInvalid || activeDeck.IsValid());
            assertX(activeDeck.isAlive());
            return activeDeck;
        }

        //##############################################################

        PooledTemplate::PooledTemplate(Card const * commander
                                      ,VCARDPOOL const & pools
                                      ,CardDB const & cardDB
                                      )
        : DeckTemplate(commander, cardDB)
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
        : DeckTemplate(commander, cardDB)
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

        ActiveDeck PooledTemplate::instantiate() const
        {
            ActiveDeck activeDeck(this->commander, this->cardDB.GetPointer());
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
                pool->GetPool(this->cardDB.GetPointer(), activeDeck.Deck);
            }

            assertX(activeDeck.IsValid());
            //std::clog << activeDeck.GetDeck() << std::endl;
            //activeDeck.PrintShort();
            return activeDeck;
        }

        //##############################################################


        QuestTemplate::QuestTemplate(Card const * commander
                                    ,VCARDPOOL const & pools
                                    ,CardDB const & cardDB
                                    ,BattleGroundEffect const battleGroundEffect
                                    )
        : PooledTemplate(commander, pools, cardDB)
        , battleGroundEffect(battleGroundEffect)
        {
        }

        BattleGroundEffect QuestTemplate::getBattleGroundEffect() const
        {
            return this->battleGroundEffect;
        }

        DeckTemplate::Ptr QuestTemplate::createFromQuestId(unsigned int const questId, CardDB const & cardDB)
        {
            assertLT(questId,STEP_MAX_ID);
            StepInfo const & stepInfo = cardDB.getQuestInfo(questId);
            assertX(stepInfo.IsValid());
            unsigned int const & commanderId(stepInfo.GetCommander());
            Card const * commander = &cardDB.GetCard(commanderId);
            BattleGroundEffect battleGroundEffect = cardDB.GetQuestEffectId(questId);

            return DeckTemplate::Ptr(new QuestTemplate(commander, stepInfo.pools, cardDB, battleGroundEffect));
        }

        DeckTemplate::Ptr createDeckTemplate(DeckArgument const & argument, CardDB const & cardDB)
        {
            switch(argument.getType()) {
                case DeckArgument::HASH:
                    if (argument.isOrdered()) {
                        return DeckTemplate::Ptr(new OrderedDeckTemplate(argument, cardDB));
                    } else {
                        return DeckTemplate::Ptr(new UnorderedDeckTemplate(argument, cardDB));
                    }
                case DeckArgument::MISSION_ID:
                    throw LogicError("Not implemented");
                case DeckArgument::RAID_ID:
                    return PooledTemplate::createFromRaidId(argument.getRaidId(), cardDB);
                case DeckArgument::QUEST_ID:
                    return QuestTemplate::createFromQuestId(argument.getQuestId(), cardDB);
                default:
                    throw LogicError("Switch Case fall through");
            }
        }

    }
}
