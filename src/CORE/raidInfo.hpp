#ifndef ITERATEDECKS_CORE_RAIDINFO_HPP
    #define ITERATEDECKS_CORE_RAIDINFO_HPP

    #include <string>
    #include "simpleTypes.hpp"
    #include "cardPool.forward.hpp"
    #include "card.forward.hpp"
    #include "playedCard.forward.hpp"

    namespace IterateDecks {
        namespace Core {

            class RaidInfo {
                public:
                    std::string Name;
                    UINT Commander;
                    VID AlwaysInclude;
                    VCARDPOOL Pools;
                public:
                    RaidInfo();
                    RaidInfo(UINT commander, const char *name);
                    RaidInfo(RaidInfo const &RI);
                    void GetAlways(Card *pCDB, LCARDS &Deck) const;
                    void GetPools(Card *pCDB, LCARDS &Deck) const;
                    const UINT GetCommander() const;
                    const char *GetName() const;
                    void AddAlways(const UINT cardID);
                    void AddPool(const CardPool &p);
                    bool IsValid() const;
                    ~RaidInfo();                    
            };

        }
    }
#endif
