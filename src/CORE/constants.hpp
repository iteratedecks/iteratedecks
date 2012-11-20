#ifndef ITERATEDECKS_CORE_CONSTANTS_HPP
    #define ITERATEDECKS_CORE_CONSTANTS_HPP

    namespace IterateDecks {
        namespace Core {

            // These should be constexpr, but Microsoft's compilers are hopelessly outdated and still do not support them.
            #define CARD_MAX_ID				4000 // sizes of storage arrays
            #define MISSION_MAX_ID			(UINT)400
            #define ACHIEVEMENT_MAX_COUNT	300
            #define RAID_MAX_ID				30
            #define BATTLEGROUND_MAX_ID		20
            #define STEP_MAX_ID				40

            #define DEFAULT_DECK_SIZE		10
            #define DEFAULT_DECK_RESERVE_SIZE	15 // up to 20? kinda deck max size for structures
            #define DEFAULT_HAND_SIZE		3

        }
    }

#endif
