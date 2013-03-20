#ifndef PLAYED_CARD_HPP_
    #define PLAYED_CARD_HPP_

    #include "playedCard.forward.hpp"

    #include "card.forward.hpp"
    #include "constants.hpp"
    #include "activeDeck.forward.hpp"

    #include "log.h"

    namespace IterateDecks {
        namespace Core {

            #define PROC50	Proc()
            bool PROC50;

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
                    UINT fsDmgDealt; // attack, counter, strike; poison is not here(hard to track the source of poison)
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
                    static unsigned int const numberOfCardLines = 6;
                    std::string toRectString(unsigned int const w, unsigned int const i) const;
                    void SetCardSkillProcBuffer(UCHAR *_SkillProcBuffer);
                    const bool GetQuestSplit() const;
                    void SetQuestSplit(bool bSplit);
                    void CardSkillProc(UCHAR aid);
                    bool BeginTurn();
                    void ProcessLegion(int count, BattleGroundEffect QuestEffectId);
                    void ProcessPoison(BattleGroundEffect QuestEffectId);
                    const UCHAR GetShield() const;
                    void ResetShield();
                    EFFECT_ARGUMENT Refresh(BattleGroundEffect QuestEffectId);
                    void ClearEnfeeble();
                    void RemoveDebuffs();
                    void Cleanse();
                    bool IsCleanseTarget();
                    bool CanEmulate(const UCHAR effect);
                    bool Regenerate(BattleGroundEffect QuestEffectId);
                    const UCHAR GetAbilitiesCount() const;
                    const UCHAR GetAbilityInOrder(const UCHAR order) const;
                    void Infuse(const UCHAR setfaction);
                    const UCHAR SufferDmg(BattleGroundEffect QuestEffectId, const UCHAR Dmg, const UCHAR Pierce = 0, UCHAR * const actualdamagedealt = 0, UCHAR *SkillProcBuffer = 0, UCHAR *OverkillDamage = 0, bool bCanRegenerate = true, VLOG *log = 0, LOG_RECORD *lr=0, bool * const damageWasDeadly = NULL);
                    bool HitCommander(BattleGroundEffect QuestEffectId,const UCHAR Dmg,PlayedCard &Src,ActiveDeck & ownDeck, ActiveDeck & otherDeck, bool isCrushDamage = false,UCHAR *overkill = NULL,VLOG *log = NULL,LOG_RECORD *lr = NULL);
                    UCHAR StrikeDmg(BattleGroundEffect const QuestEffectId, const UCHAR Dmg, UCHAR *overkill = 0); // returns dealt dmg
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
                    const CardType GetType() const;
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
                    void SetHealth(const UCHAR health);
                    void Augment(const EFFECT_ARGUMENT amount);
                    void Rally(const EFFECT_ARGUMENT amount);
                    EFFECT_ARGUMENT Weaken(const EFFECT_ARGUMENT amount);
                    void Berserk(const EFFECT_ARGUMENT amount);
                    void Protect(const EFFECT_ARGUMENT amount);
                    bool Rush(const EFFECT_ARGUMENT amount);
                    bool IsDiseased() const;
                    UCHAR Heal(EFFECT_ARGUMENT amount, BattleGroundEffect QuestEffectId = BattleGroundEffect::normal);
                    const Card *GetOriginalCard() const;
                    PlayedCard();

                    /**
                     * Copy constructor.
                     */
                    PlayedCard(PlayedCard const & original);

                    bool canAttack() const;
                    unsigned int getUniqueId() const;
            };
        }
    }
#endif
