#ifndef DECK_HPP_1
    #define DECK_HPP_1

    #include "simpleTypes.hpp"

    namespace IterateDecks { namespace Core {

    struct LOG_RECORD;
    struct LOG_CARD;

    struct REQUIREMENT {
        UCHAR SkillID;
        UCHAR Procs;
        REQUIREMENT()
        : Procs()
        { SkillID = 0; };
    };

    }}

#endif

