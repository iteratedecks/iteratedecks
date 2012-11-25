#ifndef ITERATEDECKS_CORE_SIMPLETYPES_HPP
    #define ITERATEDECKS_CORE_SIMPLETYPES_HPP

    #include <vector>
    #include <map>
    #include <string>
    #include <set>
    #include <utility> // pair
    #include "exceptions.hpp"

    namespace IterateDecks {
        namespace Core {

            typedef	unsigned char UCHAR;
            typedef	unsigned int UINT;
            typedef unsigned long DWORD;

            // that is usally the amount (i.e, the 3 in weaken 3), but also the card to summon in summon
            typedef UINT EFFECT_ARGUMENT;
            typedef UCHAR AbilityId;
            typedef UCHAR FactionId;

            // FIXME: Name scheme is inconsistent
            typedef std::vector<UINT> VID;              //< FIXME: The name seems stupid.
            typedef std::map<std::string, UINT> MSUINT;
            typedef std::vector<std::string> VSTRINGS;
            typedef std::map<std::string, UCHAR> MSKILLS;
            typedef std::map<UINT, UINT> MUUINT;
            typedef std::set<UINT> SCID;
            typedef std::pair<std::string, UINT> PAIRMSUINT;
            typedef std::pair<std::string, UCHAR> PAIRMSKILLS;
            typedef std::pair<UINT, UINT> PAIRMUUINT;


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

            enum Tag {
                TAG_ANY = -100500,
                TAG_BATCHEVAL = 100500,
                TAG_CUSTOM = 0,
                TAG_SOMERAID = -1000,
                TAG_SOMEMISSION = 1000,
            };

            inline Tag intToTag(int i) {
                switch(i) {
                    case TAG_ANY: return TAG_ANY;
                    case TAG_BATCHEVAL: return TAG_BATCHEVAL;
                    case TAG_CUSTOM: return TAG_CUSTOM;
                    case TAG_SOMERAID: return TAG_SOMERAID;
                    case TAG_SOMEMISSION: return TAG_SOMEMISSION;
                    default: throw LogicError("intToTag for unknown value");
                }
            }

            enum CardType {
                TYPE_NONE = 0,
                TYPE_COMMANDER = 1,
                TYPE_ASSAULT = 2,
                TYPE_STRUCTURE = 3,
                TYPE_ACTION = 4,
            };

            enum Faction {
                FACTION_NONE = 0,
                FACTION_IMPERIAL = 1,
                FACTION_RAIDER = 2,
                FACTION_BLOODTHIRSTY = 3,
                FACTION_XENO = 4,
                FACTION_RIGHTEOUS = 5,
            };

            enum QuestEffects {
                QEFFECT_TIME_SURGE = 1,
                QEFFECT_COPYCAT = 2,
                QEFFECT_QUICKSILVER = 3,
                QEFFECT_DECAY = 4,
                QEFFECT_HIGH_SKIES = 5,
                QEFFECT_IMPENETRABLE = 6,
                QEFFECT_INVIGORATE = 7,
                QEFFECT_CLONE_PROJECT = 8,
                QEFFECT_FRIENDLY_FIRE = 9,
                QEFFECT_GENESIS = 10,
                QEFFECT_ARTILLERY_STRIKE = 11,
                QEFFECT_PHOTON_SHIELD = 12,
                QEFFECT_ENFEEBLE_ALL = 13,
                QEFFECT_PROTECT_ALL = 14,
                QEFFECT_COMMANDER_FREEZE = 15,
                QEFFECT_SPLIT_FIVE = 16,
                QEFFECT_POISON_ALL = 17,
            };

        }
    }

#endif
