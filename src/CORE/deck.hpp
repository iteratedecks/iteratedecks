#ifndef DECK_HPP_1
    #define DECK_HPP_1

    #include "simpleTypes.hpp"

    namespace IterateDecks { namespace Core {

    class LOG_RECORD;
    class LOG_CARD;

    struct REQUIREMENT {
        UCHAR SkillID;
        UCHAR Procs;
        REQUIREMENT() { SkillID = 0; };
    };

    }}

#endif

