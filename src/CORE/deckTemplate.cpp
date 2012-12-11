#include "deckTemplate.hpp"
#include "activeDeck.hpp"
#include "cardDB.hpp"
#include "assert.hpp"

#include <iostream>

namespace IterateDecks {
    namespace Core {

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

        class OrderedDeckTemplate : public DeckTemplate {
            private:
                std::list<Card const *> cards;
            public:
                OrderedDeckTemplate(DeckArgument const & argument, CardDB const & cardDB);
                virtual ActiveDeck instantiate() const;

        };

        class UnorderedDeckTemplate : public DeckTemplate {
            private:
                std::multiset<Card const *> cards;
            public:
                UnorderedDeckTemplate(DeckArgument const & argument, CardDB const & cardDB);
                virtual ActiveDeck instantiate() const;
        };

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
            assertX(activeDeck.IsValid());
            assertX(activeDeck.isAlive());
            return activeDeck;            
        }

        DeckTemplate::DeckTemplate(CardDB const & cardDB)
        : cardDB(cardDB)
        {}

        DeckTemplate::~DeckTemplate() {}

        BattleGroundEffect DeckTemplate::getBattleGroundEffect() const
        {
            return BattleGroundEffect::normal;
        }

        DeckTemplate::Ptr DeckTemplate::create(DeckArgument const & argument, CardDB const & cardDB)
        {
            switch(argument.getType()) {
                case DeckArgument::HASH:
                    if (argument.isOrdered()) {
                        return Ptr(new OrderedDeckTemplate(argument, cardDB));
                    } else {
                        return Ptr(new UnorderedDeckTemplate(argument, cardDB));
                    }
                case DeckArgument::MISSION_ID:

                case DeckArgument::RAID_ID:

                case DeckArgument::QUEST_ID:


                default:
                    throw LogicError("Switch Case fall through");
            }
        }
    }
}
