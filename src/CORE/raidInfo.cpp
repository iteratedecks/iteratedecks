#include "raidInfo.hpp"
#include "constants.hpp"
#include "card.hpp"
#include "playedCard.hpp"
#include "cardPool.hpp"

namespace IterateDecks {
    namespace Core {

        RaidInfo::RaidInfo() {}

        RaidInfo::RaidInfo(UINT commander, char const *name)
        : Commander(commander)
        {
            if (name) {
                Name = std::string(name);
            }
            AlwaysInclude.reserve(DEFAULT_DECK_RESERVE_SIZE);
            Pools.reserve(DEFAULT_POOL_COUNT);
        }

        RaidInfo::RaidInfo(RaidInfo const &RI)
        {
            Name = RI.Name;
            Commander = RI.Commander;
            AlwaysInclude.clear();
            for (UCHAR i=0;i<RI.AlwaysInclude.size();i++)
                AlwaysInclude.push_back(RI.AlwaysInclude[i]);
            for (UCHAR i=0;i<RI.Pools.size();i++)
                Pools.push_back(RI.Pools[i]);
        }
        void RaidInfo::GetAlways(Card *pCDB, LCARDS &Deck) const
        {
            for (UCHAR i=0;i<AlwaysInclude.size();i++)
                if (pCDB[AlwaysInclude[i]].IsCard())
                    Deck.push_back(&pCDB[AlwaysInclude[i]]);
        }

        void RaidInfo::GetPools(Card *pCDB, LCARDS &Deck) const
        {
            for (UCHAR i=0;i<Pools.size();i++)
                Pools[i].GetPool(pCDB,Deck);
        }

        const UINT RaidInfo::GetCommander() const { return Commander; };

        const char *RaidInfo::GetName() const { return Name.c_str(); }

        void RaidInfo::AddAlways(const UINT cardID)
        {
            AlwaysInclude.push_back(cardID);
        }

        void RaidInfo::AddPool(const CardPool &p)
        {
            Pools.push_back(p);
        }

        bool RaidInfo::IsValid() const { return (Commander != 0); }
        
        RaidInfo::~RaidInfo()
        {
            AlwaysInclude.clear();
            Pools.clear();
        }

    }
}
