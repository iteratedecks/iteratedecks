#ifndef ITERATEDECKS_CORE_SKILL_HPP
    #define ITERATEDECKS_CORE_SKILL_HPP

    #include "constants.hpp"

    namespace IterateDecks {
        namespace Core {

            struct Skill {
                char SkillName[CARD_NAME_MAX_LENGTH];
                bool IsPassive;
                float CardValue;

                Skill();
            };

        }
    }
#endif
