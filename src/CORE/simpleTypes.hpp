#ifndef ITERATEDECKS_CORE_SIMPLETYPES_HPP
    #define ITERATEDECKS_CORE_SIMPLETYPES_HPP

    #include <vector>
    #include <map>
    #include <string>
    #include <set>
    #include <utility> // pair
    #include <sstream>
    #include "exceptions.hpp"

    namespace IterateDecks {
        namespace Core {

            typedef	unsigned char UCHAR;
            typedef	unsigned int UINT;
            typedef unsigned long DWORD;

            // that is usally the amount (i.e, the 3 in weaken 3), but also the card to summon in summon
            typedef UINT AbilityArgument;
            typedef AbilityArgument EFFECT_ARGUMENT; //< deprecated
            typedef UCHAR AbilityId; //< deprecated
            typedef UCHAR FactionId; //< deprecated
            typedef UCHAR DelayType;

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
            typedef std::multiset<UINT> MSID;

            typedef UINT CardPosition;


            /**
             * Conditions for an event, pseudo bit feld, value 0 is special.
             */
            typedef unsigned char EventCondition;
            typedef EventCondition EVENT_CONDITION; //< deprecated
            /**
             * Event conditions.
             * This is a bit field, new elements should be powers of two.
             * Zero is a special case. Would be easer if it were not.
             */
            #define EVENT_EMPTY     0
            #define EVENT_DIED      1
            #define EVENT_PLAYED    2
            #define EVENT_ATTACKED  4 // on attacked
            #define EVENT_KILL      8

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
            inline Faction idToFaction(unsigned int id)
            {
                return static_cast<Faction>(id);
            }

            enum class BattleGroundEffect {
                normal = 0,
                timeSurge = 1,
                copyCat = 2,
                quicksilver = 3,
                decay = 4,
                highSkies = 5,
                impenetrable = 6,
                invigorate = 7,
                cloneProject = 8,
                friendlyFire = 9,
                genesis = 10,
                artilleryStrike = 11,
                photonShield = 12,
                enfeebleAll = 13,
                protectAll = 14,
                commanderFreeze = 15,
                splitFive = 16,
                poisonAll = 17,
                haunt = 18,
                legionAll = 19,
            };
            typedef std::map<std::string, BattleGroundEffect> MapBattleGroundEffects;
            typedef std::pair<std::string, BattleGroundEffect> PairBattleGroundEffects;

            enum class CardActionStages {
                before,
                abilitiesDone,
                attackStarted,
                attackDone,
                cleanupDone,
            };
            inline std::ostream & operator<< (std::ostream & os, CardActionStages const & stage) {
                switch (stage) {
                    case CardActionStages::before:
                        os << "before";
                        break;
                    case CardActionStages::abilitiesDone:
                        os << "ablities done";
                        break;
                    case CardActionStages::attackStarted:
                        os << "attack started";
                        break;                        
                    case CardActionStages::attackDone:
                        os << "attack done";
                        break;
                    case CardActionStages::cleanupDone:
                        os << "cleanup done";
                        break;
                    default:
                        std::stringstream ssMessage;
                        ssMessage << "enum switch fail for constant ";
                        ssMessage << static_cast<unsigned int>(stage);
                        throw Exception(ssMessage.str());
                };
                return os;
            }
        }
    }

#endif
