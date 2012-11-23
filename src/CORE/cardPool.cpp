#include "cardPool.hpp"
#include "card.hpp"
#include "playedCard.hpp"
#include <cstdlib>
#include "assert.hpp"
#include <iostream>

namespace IterateDecks {
    namespace Core {

        // helpers
        bool IsCardInDeck(const UINT Id, LCARDS &deck)
        {
            if ((Id) && (!deck.empty()))
                for (LCARDS::const_iterator iter = deck.begin(); iter != deck.end(); iter++) {
                    if (iter->GetId() == Id) {
                        return true;
                    }
                }
            return false;
        }

        void PickACard(Card *pCDB, VID &fromIDpool, LCARDS &topool, bool checkLegendaries = true)
        {
            bool bLegendary = false;
            if (!fromIDpool.empty())
            {
                UCHAR indx = 0;
                if(checkLegendaries) {
                    for (LCARDS::iterator vi = topool.begin();vi != topool.end();vi++)
                        if (vi->GetRarity() == RARITY_LEGENDARY)
                            bLegendary = true;
                }
                UINT iPreventLoop = 0;
                do
                {
                    indx = UCHAR(rand() % fromIDpool.size());
                    iPreventLoop++;
                    if (iPreventLoop > 1000)
                    {
                        std::cout << "Looping has been prevented when choosing " <<  pCDB[fromIDpool[indx]].GetName() << "..." << std::endl;
                        break;
                    }
                }
                while (((pCDB[fromIDpool[indx]].GetRarity() == RARITY_UNIQUE) && IsCardInDeck(fromIDpool[indx],topool))
                    || (bLegendary && (pCDB[fromIDpool[indx]].GetRarity() == RARITY_LEGENDARY)));   // unique check

                topool.push_back(&pCDB[fromIDpool[indx]]);
                fromIDpool.erase(fromIDpool.begin()+indx);
            }
        };

        void CardPool::GetPool(Card * pCDB
                              ,LCARDS &Deck
                              ,bool checkLegendaries
                              ) const
        {
            assertX(Amount); // invalid pool
            assertX(!Pool.empty()); // invalid pool
            // we must copy a pool into temporary
            VID tmpPool;
            tmpPool.reserve(Pool.size());
            for (UCHAR i=0;i<Pool.size();i++) {
                tmpPool.push_back(Pool[i]);
            }
            for (UCHAR i=0;i<Amount;i++) {
                PickACard(pCDB,tmpPool,Deck,checkLegendaries);
            }
            tmpPool.clear();
        }

    }
}
