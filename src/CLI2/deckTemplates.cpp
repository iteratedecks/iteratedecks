#include "deckTemplates.hpp"
#include <list>
#include "../CORE/assert.hpp"
#include "../CORE/cardDB.hpp"
#include "../CORE/cardPool.hpp"
#include "../CORE/activeDeck.hpp"
#include <iostream>

namespace IterateDecks {
    namespace CLI {

        std::list<unsigned int> hashToId(std::string const & hash)
        {
            std::list<unsigned int> list;
            size_t len = hash.size();

            //std::clog << "parsing hash " << hash << std::endl;

            unsigned int lastid;
            for (Core::UCHAR i = 0; i < len; i+=2) {
                //std::clog << "current character is '" << hash[i] << "'" << std::endl;
                if (hash[i] == '.') break; // delimeter
                if (isspace(hash[i])) {
                    assertX(false);
                }
                unsigned int tid = 0;
                bool isCardOver4000 = false;
                if(hash[i] == '-') {
                    i++;
                    isCardOver4000 = true;
                    tid = 4000;
                }
                assertX(i + 1u < len); // make sure we have a full hash
                //std::clog << "reading characters '" << hash[i] << hash[i+1] << "' ";
                unsigned short cardHash = (hash[i] << 8) + hash[i + 1];
                tid += Core::base64ToId(cardHash);
                //std::clog << "tid is " << tid << std::endl;
                if (i==0) {
                    // first card is commander
                    assertX(tid < CARD_MAX_ID);
                    assertX((tid >= 1000) && (tid < 2000)); // commander Id boundaries
                    //std::clog << "adding commander " << tid << std::endl;
                    list.push_back(tid);
                } else {
                    // later cards are not commander
                    assertX(i>0 || (tid < CARD_MAX_ID)); // commander card can't be encoded with RLE

                    if (tid < 4000 || isCardOver4000) {
                        // this is a (non commander) card
                        list.push_back(tid);
                        //std::clog << "adding card " << tid << " the first time " << std::endl;
                        lastid = tid;
                    } else {
                        // this is a RLE
                        for (unsigned int k = 4000+1; k < tid; k++) {
                            // decode RLE, +1 because we already added one card
                            list.push_back(lastid);
                            //std::clog << "adding card " << lastid << " again " << std::endl;
                        } // for RLE
                    }
                }
            } // for

            //for(std::list<unsigned int>::const_iterator i = list.begin(); i != list.end(); i++) {
            //    std::clog << *i << " ";
            //}
            //std::clog << std::endl;
            return list;
        }

        OrderedDeckTemplate::OrderedDeckTemplate(DeckArgument const & argument, Core::CardDB const & cardDB)
        : Core::DeckTemplate()
        {
            assertX(argument.isOrdered());
            std::list<unsigned int> ids = hashToId(argument.getHash());
            std::list<unsigned int>::const_iterator iter = ids.begin();
            unsigned int const commanderId = *iter;
            this->commander = &cardDB.GetCard(commanderId);
            for(iter++ ;iter != ids.end(); iter++) {
                Core::Card const & card = cardDB.GetCard(*iter);
                this->cards.push_back(&card);
            }
        }

        Core::ActiveDeck OrderedDeckTemplate::instantiate(Core::CardDB const & cardDB) const
        {
            Core::ActiveDeck activeDeck(this->commander, cardDB.GetPointer());
            activeDeck.SetOrderMatters(true);
            for(std::list<Core::Card const *>::const_iterator iter = cards.begin(); iter != cards.end(); iter++) {
                activeDeck.Add(*iter);
            }
            assertX(activeDeck.IsValid());
            assertX(activeDeck.isAlive());
            return activeDeck;
        }

        UnorderedDeckTemplate::UnorderedDeckTemplate(DeckArgument const & argument, Core::CardDB const & cardDB)
        : Core::DeckTemplate()
        {
            assertX(!argument.isOrdered());
            std::list<unsigned int> ids = hashToId(argument.getHash());
            std::list<unsigned int>::const_iterator iter = ids.begin();
            unsigned int const commanderId = *iter;
            Core::Card const * const commanderCard = &cardDB.GetCard(commanderId);
            assertEQ(commanderCard->GetType(), Core::TYPE_COMMANDER);
            this->commander = commanderCard;
            for(iter++ ;iter != ids.end(); iter++) {
                Core::Card const & card = cardDB.GetCard(*iter);
                this->cards.insert(&card);
            }
        }

        Core::ActiveDeck UnorderedDeckTemplate::instantiate(Core::CardDB const & cardDB) const
        {
            Core::ActiveDeck activeDeck(this->commander, cardDB.GetPointer());
            activeDeck.SetOrderMatters(false);
            for(std::multiset<Core::Card const *>::const_iterator iter = cards.begin(); iter != cards.end(); iter++) {
                activeDeck.Add(*iter);
            }
            assertX(activeDeck.Commander.GetOriginalCard() != NULL);
            assertEQ(activeDeck.Commander.GetOriginalCard()->GetType(), Core::TYPE_COMMANDER);
            assertEQ(activeDeck.Commander.GetType(),Core::TYPE_COMMANDER);
            assertX(activeDeck.Commander.IsDefined());
            assertX(this->allowInvalid || activeDeck.IsValid());
            assertX(activeDeck.isAlive());
            return activeDeck;
        }

        //##############################################################

        PooledTemplate::PooledTemplate(Core::Card const * commander
                                      ,Core::VCARDPOOL const & pools
                                      ,Core::CardDB const & cardDB
                                      )
        : Core::DeckTemplate()
        , commander(commander)
        {
            for(Core::VCARDPOOL::const_iterator iter = pools.begin()
               ;iter != pools.end()
               ;iter++)
            {
                this->pools.insert(&(*iter));
            }
        }

        PooledTemplate::PooledTemplate(Core::Card const * commander
                                      ,Core::VID const & alwaysInclude
                                      ,Core::VCARDPOOL const & pools
                                      ,Core::CardDB const & cardDB
                                      )
        : Core::DeckTemplate()
        , commander(commander)
        {
            
            for(Core::VID::const_iterator iter = alwaysInclude.begin()
               ;iter != alwaysInclude.end()
               ;iter++)
            {
                unsigned int const cardId = *iter;
                Core::Card const * cardPtr = &cardDB.GetCard(cardId);
                this->alwaysInclude.insert(cardPtr);
            }

            for(Core::VCARDPOOL::const_iterator iter = pools.begin()
               ;iter != pools.end()
               ;iter++)
            {
                this->pools.insert(&(*iter));
            }
        }

        Core::DeckTemplate::Ptr PooledTemplate::createFromRaidId(unsigned int const raidId, Core::CardDB const & cardDB)
        {
            assertLT(raidId,RAID_MAX_ID);
            Core::RaidInfo const & raidInfo = cardDB.getRaidInfo(raidId);
            assertX(raidInfo.IsValid());
            unsigned int const & commanderId(raidInfo.GetCommander());
            Core::Card const * commander = &cardDB.GetCard(commanderId);

            return Core::DeckTemplate::Ptr(new PooledTemplate(commander, raidInfo.AlwaysInclude, raidInfo.Pools, cardDB));
        }

        Core::ActiveDeck PooledTemplate::instantiate(Core::CardDB const & cardDB) const
        {
            Core::ActiveDeck activeDeck(this->commander, cardDB.GetPointer());
            activeDeck.SetOrderMatters(false);
            //add alwaysinclude
            for(std::multiset<Core::Card const *>::const_iterator iter = alwaysInclude.begin()
               ;iter != alwaysInclude.end()
               ; iter++)
            {
                activeDeck.Add(*iter);
            }
            // pools
            for(std::multiset<Core::CardPool const *>::const_iterator iter = pools.begin()
               ;iter != pools.end()
               ;iter++)
            {
                Core::CardPool const * pool = *iter;
                pool->GetPool(cardDB.GetPointer(), activeDeck.Deck);
            }

            assertX(activeDeck.IsValid());
            //std::clog << activeDeck.GetDeck() << std::endl;
            //activeDeck.PrintShort();
            return activeDeck;
        }

        //##############################################################


        QuestTemplate::QuestTemplate(Core::Card const * commander
                                    ,Core::VCARDPOOL const & pools
                                    ,Core::CardDB const & cardDB
                                    )
        : PooledTemplate(commander, pools, cardDB)
        {
        }

        Core::DeckTemplate::Ptr QuestTemplate::createFromQuestId(unsigned int const questId, Core::CardDB const & cardDB)
        {
            assertLT(questId,STEP_MAX_ID);
            Core::StepInfo const & stepInfo = cardDB.getQuestInfo(questId);
            assertX(stepInfo.IsValid());
            unsigned int const & commanderId(stepInfo.GetCommander());
            Core::Card const * commander = &cardDB.GetCard(commanderId);

            return Core::DeckTemplate::Ptr(new QuestTemplate(commander, stepInfo.pools, cardDB));
        }

        Core::DeckTemplate::Ptr createDeckTemplate(DeckArgument const & argument, Core::CardDB const & cardDB)
        {
            switch(argument.getType()) {
                case DeckArgument::HASH:
                    if (argument.isOrdered()) {
                        return Core::DeckTemplate::Ptr(new OrderedDeckTemplate(argument, cardDB));
                    } else {
                        return Core::DeckTemplate::Ptr(new UnorderedDeckTemplate(argument, cardDB));
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
