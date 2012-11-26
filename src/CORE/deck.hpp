#ifndef DECK_HPP_1
    #define DECK_HPP_1

    #include "constants.hpp"

    #include "playedCard.forward.hpp"

    namespace IterateDecks { namespace Core {

    char const * const FACTIONS[6] = {0,"Imperial","Raider","Bloodthirsty","Xeno","Righteous"};

    class LOG_RECORD;
    class LOG_CARD;

    struct REQUIREMENT {
        UCHAR SkillID;
        UCHAR Procs;
        REQUIREMENT() { SkillID = 0; };
    };

    }}


    #include "playedCard.hpp"

#endif

