#include "stepInfo.hpp"
#include "cardPool.hpp"

namespace IterateDecks {
    namespace Core {

        StepInfo::StepInfo() {}

        StepInfo::StepInfo(UINT id, UINT bgid, UINT commander)
        : id(id)
        , bgId(bgid)
        , commander(commander)
        {
            this->pools.reserve(DEFAULT_POOL_COUNT);
        }

        StepInfo::StepInfo(StepInfo const & stepInfo)
        : id(stepInfo.id)
        , bgId(stepInfo.bgId)
        , commander(stepInfo.commander)
        , pools(stepInfo.pools)
        {
        }

        void StepInfo::GetPools(Card const * const pCDB, LCARDS & Deck) const
        {
            for (UCHAR i=0; i<pools.size(); i++) {
                this->pools[i].GetPool(pCDB, Deck, false);
            }
        }
        UINT StepInfo::GetCommander() const {
            return this->commander;
        }

        UINT StepInfo::GetBGId() const {
            return this->bgId;
        }

        void StepInfo::AddPool(CardPool const & p)
        {
            this->pools.push_back(p);
        }

        bool StepInfo::IsValid() const
        {
             return (this->commander != 0) && (this->id != 0) && (this->bgId != 0);
        }

        StepInfo::~StepInfo()
        {
            this->pools.clear();
        }

    }
}
