#ifndef ITERATEDECKS_CORE_CONSTANTS_HPP
    #define ITERATEDECKS_CORE_CONSTANTS_HPP

    namespace IterateDecks {
        namespace Core {

            // These should be constexpr, but Microsoft's compilers are hopelessly outdated and still do not support them.
            #define CARD_MAX_ID				5000 // sizes of storage arrays
            #define MISSION_MAX_ID			(UINT)400
            #define ACHIEVEMENT_MAX_COUNT	300
            #define RAID_MAX_ID				30
            #define BATTLEGROUND_MAX_ID		20
            #define STEP_MAX_ID				40

            #define DEFAULT_DECK_SIZE		10
            #define DEFAULT_DECK_RESERVE_SIZE	15 // up to 20? kinda deck max size for structures
            #define DEFAULT_HAND_SIZE		3

            #define CARD_NAME_MAX_LENGTH	50 // must sync it with CARD_NAME_MAX_LENGTH in interface
            #define FILENAME_MAX_LENGTH		50 //
            #define CARD_ABILITIES_MAX		70u // must sync it with CARD_ABILITIES_MAX in interface

            enum Abilities {
                SPECIAL_ATTACK = 0,
                ABILITY_ENABLED = 1,
                ACTIVATION_CHAOS = 10,
                ACTIVATION_CLEANSE,
                ACTIVATION_ENFEEBLE,
                ACTIVATION_FREEZE,
                ACTIVATION_HEAL,
                ACTIVATION_INFUSE, // this currently works only for Commander
                ACTIVATION_JAM,
                ACTIVATION_MIMIC,
                ACTIVATION_PROTECT,
                ACTIVATION_RALLY,
                ACTIVATION_RECHARGE,
                ACTIVATION_REPAIR,
                ACTIVATION_RUSH,
                ACTIVATION_SHOCK,
                ACTIVATION_SIEGE,
                ACTIVATION_SPLIT,
                ACTIVATION_STRIKE,
                ACTIVATION_SUMMON,
                ACTIVATION_SUPPLY,
                ACTIVATION_WEAKEN,
                ACTIVATION_AUGMENT,

                DEFENSIVE_ARMORED,
                DEFENSIVE_COUNTER,
                DEFENSIVE_EVADE,
                DEFENSIVE_FLYING,
                DEFENSIVE_INTERCEPT,
                DEFENSIVE_PAYBACK,
                DEFENSIVE_REFRESH,
                DEFENSIVE_REGENERATE,
                DEFENSIVE_TRIBUTE,
                DEFENSIVE_WALL,
                DEFENSIVE_EMULATE,
                DEFENSIVE_STUN,

                COMBAT_ANTIAIR,
                COMBAT_BURST,
                COMBAT_FEAR,
                COMBAT_FLURRY,
                COMBAT_PIERCE,
                COMBAT_SWIPE,
                COMBAT_VALOR,

                DMGDEPENDANT_BERSERK,
                DMGDEPENDANT_CRUSH,
                DMGDEPENDANT_DISEASE,
                DMGDEPENDANT_IMMOBILIZE,
                DMGDEPENDANT_LEECH,
                DMGDEPENDANT_POISON,
                DMGDEPENDANT_SIPHON,

                SPECIAL_BACKFIRE,  // Destroyed - When this is destroyed, deal damage to own Commander.
                SPECIAL_BLITZ,
                SPECIAL_LEGION,

                // SKILLS THAT ARE NOT DEFINED AND NOT WORKING:
                SPECIAL_FUSION, // in this sim only works for ACTIVATION skills of STRUCTURES
                SPECIAL_MIST, // this skill doesn't change anything in autoplay // max number refers to CARD_ABILITIES_MAX
                SPECIAL_BLIZZARD, // this skill doesn't change anything in autoplay // max number refers to CARD_ABILITIES_MAX
            };

            enum TargetsCount {
                TARGETSCOUNT_ONE = 0,
                TARGETSCOUNT_ALL = 10,
            };

            char const * const FACTIONS[6] = {0,"Imperial","Raider","Bloodthirsty","Xeno","Righteous"};

        }
    }

#endif
