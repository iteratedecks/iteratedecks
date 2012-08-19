#ifndef _DECK_HPP_
    #define _DECK_HPP_

    #include <vector>
    #include <list>
    #include <set>
    #include <string>

    #define CARD_NAME_MAX_LENGTH	50 // must sync it with CARD_NAME_MAX_LENGTH in interface
    #define FILENAME_MAX_LENGTH		50 //
    #define CARD_ABILITIES_MAX		70 // must sync it with CARD_ABILITIES_MAX in interface

    typedef	unsigned char UCHAR;
    typedef	unsigned int UINT;
    typedef UINT EFFECT_ARGUMENT;         // that is usally the amount (i.e, the 3 in weaken 3), but also the card to summon in summon

    class Card;
    class ActiveDeck;
    class PlayedCard;
    class LOG_RECORD;

    typedef std::vector<LOG_RECORD> VLOG;

    typedef std::vector<PlayedCard> VCARDS;
    typedef std::vector<PlayedCard*> PVCARDS;
    typedef std::pair<PlayedCard*,UCHAR> PPCARDINDEX;
    typedef std::vector<PPCARDINDEX> PPCIV;
    typedef std::multiset<UINT> MSID;
    typedef std::list<PlayedCard> LCARDS;

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
            bool HitCommander(UINT QuestEffectId,const UCHAR Dmg,PlayedCard &Src,ActiveDeck & deck,bool bCanBeCountered = true,UCHAR *overkill = NULL,VLOG *log = NULL,LOG_RECORD *lr = NULL);
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
            const bool IsDiseased() const;
            UCHAR Heal(EFFECT_ARGUMENT amount,UINT QuestEffectId = 0);
            const Card *GetOriginalCard() const;
            PlayedCard();

            /**
             * Copy constructor.
             */
            PlayedCard(PlayedCard const & original);
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
