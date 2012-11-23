#ifndef ITERATEDECKS_CORE_CARDPOOL_HPP
    #define ITERATEDECKS_CORE_CARDPOOL_HPP

    #include "cardPool.forward.hpp"

    #include "simpleTypes.hpp"
    #include <vector>
    #include "card.forward.hpp"
    #include "playedCard.forward.hpp"

    namespace IterateDecks {
        namespace Core {

            struct CardPool
            {
                UCHAR Amount;
                VID Pool;
            public:
                CardPool() {};
                CardPool(UCHAR amount)
                {
                    Amount = amount;
                    Pool.reserve(DEFAULT_POOL_SIZE);
                }
                CardPool(const CardPool &C) // copy constructor
                {
                    Amount = C.Amount;
                    Pool.clear();
                    for (UCHAR i=0;i<C.Pool.size();i++)
                        Pool.push_back(C.Pool[i]);
                }
                ~CardPool() { Pool.clear(); };
                void GetPool(Card *pCDB, LCARDS &Deck, bool checkLegendaries = true) const;
            };

        }
    }

#endif

