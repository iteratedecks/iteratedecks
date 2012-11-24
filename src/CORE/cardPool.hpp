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
                UCHAR amount;
                VID pool;
            public:
                CardPool() {};

                CardPool(UCHAR amount)
                : amount(amount)
                {
                    pool.reserve(DEFAULT_POOL_SIZE);
                }

                CardPool(CardPool const & c) // copy constructor
                : amount(c.amount)
                , pool(c.pool)
                {
                }

                ~CardPool() {};

                /**
                 * @brief Fills the deck with a random choice of cards from this pool.
                 * @param pCDB database of cards, indexed by the card id
                 * @param Deck place to put the cards
                 * @param checkLegendaries don't add a legendary to the resulting deck if there is already one
                 */
                void GetPool(Card const * pCDB, LCARDS & Deck, bool const checkLegendaries = true) const;
            };

        }
    }

#endif

