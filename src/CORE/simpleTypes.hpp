#ifndef ITERATEDECKS_CORE_SIMPLETYPES_HPP
    #define ITERATEDECKS_CORE_SIMPLETYPES_HPP

    #include <vector>

    namespace IterateDecks {
        namespace Core {

            typedef	unsigned char UCHAR;
            typedef	unsigned int UINT;

            // that is usally the amount (i.e, the 3 in weaken 3), but also the card to summon in summon
            typedef UINT EFFECT_ARGUMENT;
            typedef UCHAR AbilityId;
            typedef UCHAR FactionId;

            typedef std::vector<UINT> VID;

            /**
             * Conditions for an event, pseudo bit feld, value 0 is special.
             */
            typedef unsigned char EVENT_CONDITION;
            /**
             * Event conditions.
             * This is a bit field, new elements should be powers of two.
             * Zero is a special case. Would be easer if it were not.
             */
            #define EVENT_EMPTY				0
            #define EVENT_DIED				1
            #define EVENT_PLAYED			2
            //#define EVENT_BOTH				3 // bad name, but means: on play on death. no longer used.
            #define EVENT_ATTACKED          4 // on attacked
            // next EVENT_SOMETHING         8

            enum Rarity {
                RARITY_COMMON = 0,
                RARITY_UNCOMMON = 1,
                RARITY_RARE = 2,
                RARITY_UNIQUE = 3,
                RARITY_LEGENDARY = 4,
                RARITY_STORYCOMMANDER = 10,
            };

        }
    }

#endif
