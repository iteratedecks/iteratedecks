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

            #define CARD_NAME_MAX_LENGTH	50 // must sync it with CARD_NAME_MAX_LENGTH in interface
            #define FILENAME_MAX_LENGTH		50 //
            #define CARD_ABILITIES_MAX		70u // must sync it with CARD_ABILITIES_MAX in interface

            // TODO: Make this an enum
            #define ABILITY_ENABLED			1  // just a helper, no use

            #define SPECIAL_ATTACK			0

            #define ACTIVATION_CHAOS		11
            #define ACTIVATION_CLEANSE		12
            #define ACTIVATION_ENFEEBLE		13
            #define ACTIVATION_FREEZE		14
            #define ACTIVATION_HEAL			15
            #define ACTIVATION_INFUSE		16	// this currently works only for Commander
            #define ACTIVATION_JAM			17
            #define ACTIVATION_MIMIC		18
            #define ACTIVATION_PROTECT		19
            #define ACTIVATION_RALLY		20
            #define ACTIVATION_RECHARGE		21
            #define ACTIVATION_REPAIR		22
            #define ACTIVATION_RUSH			23
            #define ACTIVATION_SHOCK		24
            #define ACTIVATION_SIEGE		25
            #define ACTIVATION_SPLIT		26
            #define ACTIVATION_STRIKE		27
            #define ACTIVATION_SUMMON       28 // i think 30 is still free... these numbers do not have any other meaning? why not use an enum?
            #define ACTIVATION_SUPPLY		29
            #define ACTIVATION_WEAKEN		30
            #define ACTIVATION_AUGMENT		63

            #define DEFENSIVE_ARMORED		31
            #define DEFENSIVE_COUNTER		32
            #define DEFENSIVE_EVADE			33
            #define DEFENSIVE_FLYING		34
            #define DEFENSIVE_INTERCEPT		35
            #define DEFENSIVE_PAYBACK		36
            #define DEFENSIVE_REFRESH		37
            #define DEFENSIVE_REGENERATE	38
            #define DEFENSIVE_TRIBUTE		39
            #define DEFENSIVE_WALL			40

            #define COMBAT_ANTIAIR			41
            #define COMBAT_BURST			42
            #define COMBAT_FEAR				43
            #define COMBAT_FLURRY			44
            #define COMBAT_PIERCE			45
            #define COMBAT_SWIPE			46
            #define COMBAT_VALOR			47

            #define DMGDEPENDANT_BERSERK	51
            #define DMGDEPENDANT_CRUSH		52
            #define DMGDEPENDANT_DISEASE	53
            #define DMGDEPENDANT_IMMOBILIZE	54
            #define DMGDEPENDANT_LEECH		55
            #define DMGDEPENDANT_POISON		56
            #define DMGDEPENDANT_SIPHON		57

            #define SPECIAL_BACKFIRE		61  // Destroyed - When this is destroyed, deal damage to own Commander.
            #define SPECIAL_BLITZ			66 // showed up in cards.xml; not implemented

            // SKILLS THAT ARE NOT DEFINED AND NOT WORKING:
            #define SPECIAL_FUSION			62	// in this sim only works for ACTIVATION skills of STRUCTURES
            #define SPECIAL_MIST			64 // this skill doesn't change anything in autoplay // max number refers to CARD_ABILITIES_MAX
            #define SPECIAL_BLIZZARD		65 // this skill doesn't change anything in autoplay // max number refers to CARD_ABILITIES_MAX

            // TODO: Make this an enum
            #define TARGETSCOUNT_ONE		0
            #define TARGETSCOUNT_ALL		10

        }
    }

#endif
