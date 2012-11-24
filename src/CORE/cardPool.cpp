#include "cardPool.hpp"
#include "card.hpp"
#include "playedCard.hpp"
#include <cstdlib>
#include "assert.hpp"
#include <iostream>

namespace IterateDecks {
    namespace Core {

        // helpers
        /**
         * @brief Checks whether a card is inside a deck.
         * @param id the card's id
         * @param deck a list of cards
         * @return whether a card with the given id is insede the deck
         */
        bool isCardInDeck(UINT const id
                         ,LCARDS const & deck
                         )
        {
            // I don't see any reason why we would call this with an id of 0
            assertX(id);
            if(!id) {
                return false;
            }
            // TODO does this check improve speed or not?
            if (!deck.empty()) {
                for (LCARDS::const_iterator iter = deck.begin(); iter != deck.end(); iter++) {
                    if (iter->GetId() == id) {
                        return true;
                    }
                }
            }
            return false;
        }

        void PickACard(Card const * pCDB
                      ,VID & fromIdPool
                      ,LCARDS & toPool
                      ,bool const checkLegendaries
                      )
        {
            if (!fromIdPool.empty()) {
                bool bLegendary = false;    //< stores whether we already have a legendary

                if(checkLegendaries) {
                    // Check if we already have a legendary.
                    for (LCARDS::iterator vi = toPool.begin(); vi != toPool.end(); vi++) {
                        if (vi->GetRarity() == RARITY_LEGENDARY) {
                            bLegendary = true;
                        }
                    }
                }

                int maxTries = 1000; //< abort after too many tries
                UCHAR id = 0;     //< id of found card
                UINT index = 0xdeadbeef;
                Card card;
                do {
                    // Note that this is not a uniform distribution unless
                    // fromIdPool.size() is much smaller than RAND_MAX
                    // (which we can assume) or fromIdPool.size() divides RAND_MAX
                    index = (rand() % fromIdPool.size());
                    id = fromIdPool[index];
                    card = pCDB[id];

                    maxTries--;
                    if (maxTries <= 0) {
                        std::cout << "Looping has been aborted when choosing " <<  card.GetName() << "..." << std::endl;
                        break;
                    }
                }
                while (((card.GetRarity() == RARITY_UNIQUE) && isCardInDeck(id,toPool))
                    || (bLegendary && (card.GetRarity() == RARITY_LEGENDARY)));   // unique check

                toPool.push_back(&card);
                fromIdPool.erase(fromIdPool.begin()+index);
            }
        };

        void CardPool::GetPool(Card const * pCDB
                              ,LCARDS & Deck
                              ,bool const checkLegendaries
                              ) const
        {
            assertX(this->amount); // invalid pool
            assertX(!this->pool.empty()); // invalid pool
            // we must copy a pool into temporary
            VID tmpPool(this->pool);

            for (UCHAR i=0; i<this->amount; i++) {
                PickACard(pCDB, tmpPool, Deck, checkLegendaries);
            }
        }

    }
}
