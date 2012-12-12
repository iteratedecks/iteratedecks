#ifndef ITERATEDECKS_CORE_STEPINFO_HPP
    #define ITERATEDECKS_CORE_STEPINFO_HPP

    #include "simpleTypes.hpp"
    #include "cardPool.forward.hpp"
    #include "card.forward.hpp"
    #include "playedCard.forward.hpp"

    namespace IterateDecks {
        namespace Core {

            class StepInfo {
                public:
                    UINT id;
                    UINT bgId;
                    UINT commander;
                    VCARDPOOL pools;
                public:
                    StepInfo();
                    StepInfo(UINT id, UINT bgid, UINT commander);
                    StepInfo(StepInfo const &SI);
                    void GetPools(Card const * const pCDB, LCARDS &Deck) const;
                    UINT GetCommander() const;
                    UINT GetBGId() const;
                    void AddPool(CardPool const & p);
                    bool IsValid() const;
                    ~StepInfo();
            };

        }
    }

#endif
