#ifndef ITERATEDECKS_CORE_BGINFO_HPP
    #define ITERATEDECKS_CORE_BGINFO_HPP

    #include "simpleTypes.hpp"
    #include <string>

    namespace IterateDecks {
        namespace Core {

            class BgInfo {
                private:
                    UINT id;
                    std::string name;
                    std::string desc;
                    BattleGroundEffect effectId;
                public:
                    BgInfo();
                    BgInfo(UINT const id,char const * const name, char const * const desc, BattleGroundEffect const effectid);
                    BgInfo(BgInfo const & BI);
                    std::string GetName();
                    BattleGroundEffect GetEffectId();
            };

        }
    }
#endif
