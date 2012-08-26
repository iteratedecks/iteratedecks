#ifndef _DECK_HPP_
    #define _DECK_HPP_

    #include <vector>
    #include <list>
    #include <set>
    #include <string>

    #define CARD_NAME_MAX_LENGTH	50 // must sync it with CARD_NAME_MAX_LENGTH in interface
    #define FILENAME_MAX_LENGTH		50 //
    #define CARD_ABILITIES_MAX		70 // must sync it with CARD_ABILITIES_MAX in interface

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

    // SKILLS THAT ARE NOT DEFINED AND NOT WORKING:
    #define SPECIAL_FUSION			62	// in this sim only works for ACTIVATION skills of STRUCTURES
    #define SPECIAL_AUGMENT			63
    #define SPECIAL_MIST			64 // this skill doesn't change anything in autoplay // max number refers to CARD_ABILITIES_MAX
    #define SPECIAL_BLIZZARD		65 // this skill doesn't change anything in autoplay // max number refers to CARD_ABILITIES_MAX

    typedef	unsigned char UCHAR;
    typedef	unsigned int UINT;
    typedef UINT EFFECT_ARGUMENT;         // that is usally the amount (i.e, the 3 in weaken 3), but also the card to summon in summon
    typedef UCHAR AbilityId;
    typedef UCHAR FactionId;
    char const * const FACTIONS[6] = {0,"Imperial","Raider","Bloodthirsty","Xeno","Righteous"};

    class Card;
    class ActiveDeck;
    class PlayedCard;
    class LOG_RECORD;
    class CardDB;

    typedef std::vector<LOG_RECORD> VLOG;

    typedef std::vector<PlayedCard> VCARDS;
    typedef std::vector<PlayedCard*> PVCARDS;
    typedef std::pair<PlayedCard*,UCHAR> PPCARDINDEX;
    typedef std::vector<PPCARDINDEX> PPCIV;
    typedef std::multiset<UINT> MSID;
    typedef std::list<PlayedCard> LCARDS;

    #include "playedCard.hpp"
    class PlayedCard {
        private:
            // constant
            const Card *OriginalCard;
            // temporary
            UCHAR Attack;
            UCHAR Health;
            UCHAR Wait;
            UCHAR Faction; // this is for Infuse
            UCHAR Effects[CARD_ABILITIES_MAX];
            bool bPlayed;
            bool bActivated;
            bool bQuestSplit;
            UCHAR DeathEvents;
        public:
            // fancy stats
            UINT fsDmgDealt; // attack, counter, strike, poison is not here(hard to track the source of poison)
            UINT fsDmgMitigated; // obvious - before it died, should we include overkill?
            UINT fsAvoided; // evade, armor, flying, protect - absorbed damage, this always IGNORES protect and ignores armor if it's a flying miss
            UINT fsHealed; // supply, heal, leech, regenerate
            UINT fsSpecial; // enfeeble, protect, weaken, rally, poison(when applied)
            UINT fsOverkill; // overkill damage
            UINT fsDeaths;
            // skill proc counter
            UCHAR *SkillProcBuffer;
        private:
            static unsigned int nextUniqueId;
            unsigned int uniqueId;
        public:
            void DecWait();
            void IncWait();
            const char *GetName() const;
            void PrintDesc();
            std::string toString() const;
            void SetCardSkillProcBuffer(UCHAR *_SkillProcBuffer);
            const bool GetQuestSplit() const;
            void SetQuestSplit(bool bSplit);
            void CardSkillProc(UCHAR aid);
            const bool BeginTurn();
            void ProcessPoison(UINT QuestEffectId);
            const UCHAR GetShield() const;
            void ResetShield();
            void Refresh(UINT QuestEffectId);
            void ClearEnfeeble();
            void RemoveDebuffs();
            void Cleanse();
            bool IsCleanseTarget();
            void EndTurn();
            const UCHAR GetAbilitiesCount() const;
            const UCHAR GetAbilityInOrder(const UCHAR order) const;
            void Infuse(const UCHAR setfaction);
            const UCHAR SufferDmg(UINT QuestEffectId, const UCHAR Dmg, const UCHAR Pierce = 0, UCHAR *actualdamagedealt = 0, UCHAR *SkillProcBuffer = 0, UCHAR *OverkillDamage = 0, bool bCanRegenerate = true, VLOG *log = 0, LOG_RECORD *lr=0);
            bool HitCommander(UINT QuestEffectId,const UCHAR Dmg,PlayedCard &Src,ActiveDeck & ownDeck, ActiveDeck & otherDeck, bool bCanBeCountered = true,UCHAR *overkill = NULL,VLOG *log = NULL,LOG_RECORD *lr = NULL);
            UCHAR StrikeDmg(const UINT QuestEffectId, const UCHAR Dmg, UCHAR *overkill = 0); // returns dealt dmg
            const bool IsAlive() const;
            const bool IsDefined() const;
            bool OnDeathEvent();
            const UCHAR GetRarity() const;
            bool operator==(const PlayedCard &C) const;
            bool operator!=(const PlayedCard &C) const;
            bool operator<(const PlayedCard &C) const;
            PlayedCard& operator=(const Card *card);
            PlayedCard(const Card *card);
            const UINT GetId() const;
            const UCHAR GetNativeAttack() const;
            const UCHAR GetAttack() const;
            const UCHAR GetHealth() const;
            const UCHAR GetMaxHealth() const;
            const UCHAR GetFaction() const;
            const UCHAR GetWait() const;
            const UCHAR GetType() const;
            const UCHAR GetEffect(const UCHAR id) const;
            const EFFECT_ARGUMENT GetAbility(const UCHAR id) const;
            const UCHAR GetTargetCount(const UCHAR id) const;
            const UCHAR GetTargetFaction(const UCHAR id) const;
            const UCHAR GetAbilityEvent(const UCHAR id) const;
            const bool GetPlayed() const;
            void Played();
            void ResetPlayedFlag();
            void SetAttack(const UCHAR attack);
            void SetEffect(const UCHAR id, const UCHAR value);
            void Rally(const EFFECT_ARGUMENT amount);
            EFFECT_ARGUMENT Weaken(const EFFECT_ARGUMENT amount);
            void Berserk(const EFFECT_ARGUMENT amount);
            void Protect(const EFFECT_ARGUMENT amount);
            bool Rush(const EFFECT_ARGUMENT amount);
            bool IsDiseased() const;
            UCHAR Heal(EFFECT_ARGUMENT amount,UINT QuestEffectId = 0);
            const Card *GetOriginalCard() const;
            PlayedCard();

            /**
             * Copy constructor.
             */
            PlayedCard(PlayedCard const & original);

            bool canAttack() const;
    };


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

#endif
