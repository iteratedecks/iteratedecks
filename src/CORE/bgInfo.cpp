#include "bgInfo.hpp"

namespace IterateDecks {
    namespace Core {
        BgInfo::BgInfo() {};

        BgInfo::BgInfo(UINT const id
                      ,char const * const name
                      ,char const * const desc
                      ,UINT const effectid
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

        const char * BgInfo::GetName()
        {
            return this->name.c_str();
        }
        UINT BgInfo::GetEffectId()
        {
            return this->effectId;
        }
    }
}
