#include "bgInfo.hpp"

namespace IterateDecks {
    namespace Core {
        BgInfo::BgInfo() {};

        BgInfo::BgInfo(UINT const id
                      ,char const * const name
                      ,char const * const desc
                      ,BattleGroundEffect const effectid
                      )
        : id(id)
        , name(name)
        , desc(desc)
        , effectId(effectId)
        {
        }

        BgInfo::BgInfo(BgInfo const & bgInfo) // copy constructor
        : id(bgInfo.id)
        , name(bgInfo.name)
        , desc(bgInfo.desc)
        , effectId(bgInfo.effectId)
        {
        }

        std::string BgInfo::GetName()
        {
            return this->name;
        }

        BattleGroundEffect BgInfo::GetEffectId()
        {
            return this->effectId;
        }
    }
}
