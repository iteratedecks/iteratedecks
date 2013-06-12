#include "playedCard.hpp"
#include <sstream>
#include <iomanip>
#include "card.hpp"
#include <iostream>
#include "assert.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "exceptions.hpp"
#include "Logger.hpp"

namespace IterateDecks {
    namespace Core {

        // FIXME: remove that first generation debugging stuff:
        extern bool bConsoleOutput;

        #define PROC50	Proc()
        bool PROC50
        {
            return (rand()%100 < 50);
        }

        bool PlayedCard::canAttack() const {
            if(this->GetEffect(DMGDEPENDANT_IMMOBILIZE) > 0) {
                return false;
            } else if (this->GetEffect(ACTIVATION_JAM) > 0) {
                return false;
            } else if (this->GetEffect(ACTIVATION_FREEZE) > 0) {
                return false;
            } else if (this->GetAttack() <= 0) {
                return false;
            } else if (this->GetEffect(DEFENSIVE_STUN) > 0) {
                return false;
            }
            return true;
        }

        unsigned int PlayedCard::getUniqueId() const {
            return this->uniqueId;
        }

        //unsigned int const PlayedCard::numberOfCardLines(6U);

        std::string PlayedCard::toRectString(unsigned int const w, unsigned int const i) const
        {
            std::stringstream ssLine;
            unsigned int const type = this->GetType();
            switch(i) {
                case 0: case 5: {
                    ssLine << "+" << std::setw(w-2);
                    ssLine << std::setfill('-') << "" << "+";
                } break;
                case 1: {
                    ssLine << "|";
                    ssLine << std::setw(4) << this->getUniqueId();
                    ssLine << std::setw(w-4-2-2) << "";
                    if (this->GetWait() > 0) {
                        ssLine << std::setw(2) << (unsigned int)this->GetWait();
                    } else {
                        ssLine << "  ";
                    }
                    ssLine << "|";
                } break;
                case 2: {
                    ssLine << "|" << std::setw(w-2) << std::left << this->GetName() << "|";
                } break;
                case 3: {
                    ssLine << "|";
                    int l = w-2;
                    if(!this->IsAlive()) {
                        ssLine << "Dead ";
                        l-=5;
                    }
                    if(this->IsDiseased()) {
                        ssLine << "D ";
                        l-=2;
                    }
                    if(this->GetEffect(DMGDEPENDANT_POISON)) {
                        ssLine << "Po" << std::setw(2) << (UINT)this->GetEffect(DMGDEPENDANT_POISON) << " ";
                        l-=2+2+1;
                    }
                    ssLine << std::setw(l) << "";
                    ssLine << "|";
                } break;
                case 4: {
                    ssLine << "|";
                    ssLine << std::setw(w-2-5) << "";
                    ssLine << std::setw(2);
                    if(type==TYPE_ASSAULT) {
                        ssLine << (unsigned int)this->GetAttack();
                        ssLine << "/";
                    } else {
                        ssLine << "";
                        ssLine << " ";
                    }
                    ssLine << std::setw(2) << (unsigned int)this->GetHealth();
                    ssLine << "|";
                } break;
            }
            return ssLine.str();
        }

        void
        PlayedCard::DecWait()
        {
            if ((Wait > 0) && (Effects[ACTIVATION_FREEZE] == 0)) {
                Wait--;
            }
            this->stage = CardActionStages::before;
        }

        void PlayedCard::IncWait() { if (Wait) Wait++; }  // this is only for tournaments

        const char * PlayedCard::GetName() const { return OriginalCard->GetName(); }

        void PlayedCard::PrintDesc()
        {
            if (IsDefined())
            {
                std::cout << this->toString();
            }
        }
        std::string PlayedCard::toString() const
        {
            if (!IsDefined()) {
                assertX(IsDefined());
                return "<undefined>";
            } else {
                std::stringstream ss;
                ss << "[";
                ss << std::setw(4) << this->uniqueId;
                ss << "|";
                switch(this->GetType()){
                    case TYPE_ACTION   : ss << 'A'; break;
                    case TYPE_ASSAULT  : ss << 'U'; break;
                    case TYPE_COMMANDER: ss << 'C'; break;
                    case TYPE_STRUCTURE: ss << 'S'; break;
                    default: throw LogicError("toString: Unknown card type");
                }
                ss << " \"" << OriginalCard->GetName() << '"';
                if (this->GetType()!=TYPE_ACTION) {
                    ss << " ";
                    if (this->GetType()==TYPE_ASSAULT) {
                        ss << "A:";
                        ss << (UINT)this->GetAttack();
                        ss << " ";
                    }
                    ss << "H:";
                    ss << (UINT)this->Health;
                    if (this->GetType()!=TYPE_COMMANDER) {
                        ss << " D:";
                        ss << (UINT)this->Wait;
                    }
                    if(this->GetType()==TYPE_ACTION) {
                        if (Effects[ACTIVATION_JAM]) {
                            ss << " Jammed";
                        }
                        if (Effects[ACTIVATION_FREEZE]) {
                            ss << " Frozen";
                        }
                        if (Effects[DMGDEPENDANT_IMMOBILIZE]) {
                            ss << " Immobilized";
                        }
                        if (Effects[DMGDEPENDANT_DISEASE]) {
                            ss << " Diseased";
                        }
                        if (Effects[DMGDEPENDANT_SUNDER]) {
                            ss << " Sundered";
                        }
                        if (Effects[ACTIVATION_CHAOS]) {
                            ss << " Chaosed";
                        }
                    }
                }
                ss << "]";
                return ss.str();
            }
        }
        void PlayedCard::SetCardSkillProcBuffer(UCHAR *_SkillProcBuffer)
        {
            SkillProcBuffer = _SkillProcBuffer;
        }
        const bool PlayedCard::GetQuestSplit() const
        {
            return bQuestSplit;
        }
        void PlayedCard::SetQuestSplit(bool bSplit)
        {
            bQuestSplit = bSplit;
        }
        void PlayedCard::CardSkillProc(UCHAR aid)
        {
            if (SkillProcBuffer) {
                SkillProcBuffer[aid]++;
            }
        }
        bool PlayedCard::BeginTurn()
        {
            const bool bDoBegin = (Health>0)
                && (!Effects[ACTIVATION_JAM])
                && (!Effects[ACTIVATION_FREEZE])
                && ((Wait <= 0) || (Effects[SPECIAL_BLITZ] > 0));
            // P: Wait, that seems wrong. Blitz should only trigger if the opposite card is active...

            // Assume all cards Blitzing are treated exactly as if they were active;
            // we need to reset this to False after we remove Blitz
            if (bDoBegin && (!bActivated))
                bActivated = true;
            return bDoBegin;
        }
        void PlayedCard::ProcessLegion(int count, BattleGroundEffect QuestEffectId)
        {
            if (count > 0 && IsAlive() && (GetAbility(SPECIAL_LEGION))) {
                int amount = count * GetAbility(SPECIAL_LEGION);
                this->Rally(amount);
                this->Heal(amount, QuestEffectId);
            }
        }
        void PlayedCard::ProcessPoison(BattleGroundEffect QuestEffectId, DeckLogger * const logger)
        {
            EFFECT_ARGUMENT amount = this->Effects[DMGDEPENDANT_POISON];
            if (this->IsAlive() && (amount > 0)) {
                LOG(logger, cardDamaged(*this,DMGDEPENDANT_POISON,amount));
                this->SufferDmg(QuestEffectId, amount);
            }
        }
        const UCHAR PlayedCard::GetShield() const
        {
            return Effects[ACTIVATION_PROTECT];
        }
        void PlayedCard::ResetShield()
        {
            Effects[ACTIVATION_PROTECT] = 0;
        }

        EFFECT_ARGUMENT PlayedCard::Refresh(BattleGroundEffect QuestEffectId)
        {
            UCHAR amount = OriginalCard->GetHealth() - this->Health;
            if (QuestEffectId == BattleGroundEffect::invigorate) {
                this->Attack += amount;
            }
            fsHealed += amount;
            if (SkillProcBuffer && (amount != 0)) {
                SkillProcBuffer[DEFENSIVE_REFRESH]++;
            }
            this->Health = OriginalCard->GetHealth();
            return amount;
        }

        void PlayedCard::ClearEnfeeble()
        {
            Effects[ACTIVATION_ENFEEBLE] = 0;
        }
        void PlayedCard::RemoveDebuffs()
        {
    /*
    by Moraku:
    Chaos: Removed after owner ends his turn.
    Disease: Never removed (unless cleansed).
    Enfeeble: Removed after either player ends his turn.
    Fusion: Never removed (unless less than 3 Fusion cards active).
    Immobilize: Removed after owner ends his turn.
    Infusion: Never removed.
    Jam: Removed after owner ends his turn.
    Poison: Never removed (unless cleansed).
    Protect: Removed before owner begins his turn.
    Rally: Removed after owner ends his turn.
    Weaken: Removed after owner ends his turn.
    Valor: Removed after owner ends his turn.
    */
            Effects[ACTIVATION_JAM] = 0;
            Effects[ACTIVATION_FREEZE] = 0; // is it removed at the end of turn?
            Effects[DMGDEPENDANT_IMMOBILIZE] = 0;
            Effects[ACTIVATION_CHAOS] = 0;
            // really?
            Effects[ACTIVATION_RALLY] = 0;
            Effects[ACTIVATION_WEAKEN] = 0;
            Effects[ACTIVATION_AUGMENT] = 0;

            if(Effects[DEFENSIVE_STUN] > 0) {
                Effects[DEFENSIVE_STUN]--;
            }

            if(Effects[SPECIAL_BLITZ]) {
                Effects[SPECIAL_BLITZ] = 0;
                bActivated = false;
            }
        }
        void PlayedCard::Cleanse()
        {
            Effects[DMGDEPENDANT_POISON] = 0;
            Effects[DMGDEPENDANT_DISEASE] = 0;
            Effects[DMGDEPENDANT_SUNDER] = 0;
            Effects[ACTIVATION_JAM] = 0;
            if (bActivated)  // this is bullshit!
                Effects[ACTIVATION_FREEZE] = 0;
            Effects[DMGDEPENDANT_IMMOBILIZE] = 0;
            Effects[ACTIVATION_ENFEEBLE] = 0;
            Effects[ACTIVATION_CHAOS] = 0;
            Effects[DEFENSIVE_STUN] = 0;
        }
        bool PlayedCard::IsCleanseTarget()
        {
            // Poison, Disease, Jam, Immobilize, Enfeeble, Chaos. (Does not remove Weaken.)
            return (Effects[DMGDEPENDANT_POISON] ||
                    Effects[DMGDEPENDANT_DISEASE] ||
                    Effects[DMGDEPENDANT_SUNDER] ||
                    Effects[ACTIVATION_JAM] ||
                    (Effects[ACTIVATION_FREEZE] && bActivated) ||
                    Effects[DMGDEPENDANT_IMMOBILIZE] ||
                    Effects[ACTIVATION_ENFEEBLE] ||
                    Effects[ACTIVATION_CHAOS] ||
                    Effects[DEFENSIVE_STUN]);
        }
        bool PlayedCard::CanEmulate(const UCHAR effect)
        {
            if(!(this->GetAbility(DEFENSIVE_EMULATE))) return false;

            if(effect == ACTIVATION_AUGMENT
                || effect == ACTIVATION_CLEANSE
                || (effect == ACTIVATION_HEAL && !IsDiseased())
                || effect == ACTIVATION_PROTECT
                || (effect == ACTIVATION_RALLY && !IsSundered())
                || effect == ACTIVATION_REPAIR
                || effect == ACTIVATION_RUSH
                || (effect == ACTIVATION_SUPPLY && !IsDiseased())) {
                    return true;
            }

            return false;
        }
        bool PlayedCard::Regenerate(BattleGroundEffect QuestEffectId, DeckLogger * const logger) {
            // regnerate?
            EFFECT_ARGUMENT const regenerateAmount = this->OriginalCard->GetAbility(DEFENSIVE_REGENERATE);
            if (!this->IsDiseased() && (regenerateAmount > 0) && (PROC50))
            {
                // This unit regenerates.
                this->Health = regenerateAmount;
                fsHealed += regenerateAmount;
                if (QuestEffectId == BattleGroundEffect::invigorate) {
                    this->Attack += regenerateAmount;
                }
                CardSkillProc(DEFENSIVE_REGENERATE);
                LOG(logger,cardRegenerated(*this, regenerateAmount));
                return true;
            } else {
                // This unit does not regenerate
                if (IsAlive()) { // shouldn't die twice ;)
                    fsDeaths++;
                }
                this->Health = 0;
                LOG(logger,cardDestroyed(*this));
                return false;
            }
        }

        const UCHAR PlayedCard::GetAbilitiesCount() const { return OriginalCard->GetAbilitiesCount(); }
        const UCHAR PlayedCard::GetAbilityInOrder(const UCHAR order) const { return OriginalCard->GetAbilityInOrder(order); }

        void
        PlayedCard::Infuse(Faction setfaction)
        {
            // this is kind of a kludge...
            SetEffect(ACTIVATION_INFUSE, FACTION_BLOODTHIRSTY);
            //OriginalCard.Infuse(setfaction);
            this->faction = setfaction;
            if(this->SkillProcBuffer) {
                SkillProcBuffer[ACTIVATION_INFUSE]++;
            }
        }

        const UCHAR PlayedCard::SufferDmg(BattleGroundEffect QuestEffectId
                                         ,UCHAR const Dmg
                                         ,UCHAR const Pierce
                                         ,UCHAR * const actualdamagedealt
                                         ,UCHAR * SkillProcBuffer
                                         ,UCHAR * OverkillDamage
                                         ,LOG_RECORD *lr
                                         ,bool * const damageWasDeadly // needed for crush
                                         )
        {
            assertX(this->OriginalCard);
    // Regeneration happens before the additional strikes from Flurry.
    // Regenerating does not prevent Crush damage
            UCHAR dmg = Dmg;
            EFFECT_ARGUMENT shield = (EFFECT_ARGUMENT)GetEffect(ACTIVATION_PROTECT);
            if (Pierce >= shield) {
                dmg = Dmg;
            } else if (Pierce+Dmg >= shield) {
                dmg = Pierce + Dmg - shield;
            } else {
                dmg = 0;
            }
            if (lr) {
                lr->Effect = dmg;
            }
            if (dmg >= Health) {
                // This attack is deadly
                if(damageWasDeadly != NULL) {
                    *damageWasDeadly = true;
                }

                // Store overkill damage
                if (OverkillDamage != NULL) {
                    *OverkillDamage += (dmg - Health);
                }

                // We deal as much as the unit has
                UCHAR dealt = Health;

                this->Health = 0;

                DeathEvents++;
                if (actualdamagedealt) // siphon and leech are kinda bugged - overkill damage counts as full attack damage even if card has 1 hp left, therefore this workaround
                {
                    *actualdamagedealt = dealt;
                    return dmg;
                }
                // crush damage will be dealt even if the defending unit Regenerates
                return dealt;// note that CRUSH & BACKFIRE are processed elsewhere
            } else {
                // This attack is not deadly
                if(damageWasDeadly != NULL) {
                    *damageWasDeadly = false;
                }

                this->Health -= dmg;
                if (actualdamagedealt) {
                    *actualdamagedealt = dmg;
                }
            }
            fsDmgMitigated += dmg;
            return dmg;
        }

        UCHAR PlayedCard::StrikeDmg(BattleGroundEffect const QuestEffectId, const UCHAR Dmg, UCHAR *overkill) // returns dealt dmg
        {
            assertX(Dmg); // 0 dmg is pointless and indicates an error
            //printf("%s %d <- %d\n",GetName(),GetHealth(),Dmg);
            return SufferDmg(QuestEffectId,Dmg + Effects[ACTIVATION_ENFEEBLE],0,0,0,overkill);
        }
        const bool PlayedCard::IsAlive() const
        {
            return ((OriginalCard) &&/* THIS IS CRAP! */(Health != 0)); //(Attack||Health||Wait);
        }
        const bool PlayedCard::IsDefined() const
        {
            //return (OriginalCard && (Attack||Health||Wait||(GetType() == TYPE_COMMANDER)||(GetType() == TYPE_ACTION)));
            return (    (OriginalCard != NULL)
                     && (GetType() != TYPE_NONE)
                   );
        }
        bool PlayedCard::OnDeathEvent()
        {
            if (DeathEvents > 0)
            {
                DeathEvents--;
                return true;
            }
            return false;
        }
        const UCHAR PlayedCard::GetRarity() const
        {
            return OriginalCard->GetRarity();
        }
        bool PlayedCard::operator==(const PlayedCard &C) const
        {
            if (OriginalCard != C.OriginalCard) // better to compare ID's, but this should also work
                return false;
            if (Wait != C.Wait)
                return false;
            if (Attack != C.Attack)
                return false;
            if (Health != C.Health)
                return false;
            if (bPlayed != C.bPlayed)
                return false;
            if (bActivated != C.bActivated)
                return false;
            if (isSummoned != C.isSummoned)
                return false;
            if (faction != C.faction)
                return false;
            return (!memcmp(Effects,C.Effects,CARD_ABILITIES_MAX * sizeof(UCHAR)));
        }
        bool PlayedCard::operator!=(const PlayedCard &C) const
        {
            if (OriginalCard != C.OriginalCard) // better to compare ID's, but this should also work
                return true;
            if (Wait != C.Wait)
                return true;
            if (Attack != C.Attack)
                return true;
            if (Health != C.Health)
                return true;
            if (bPlayed != C.bPlayed)
                return true;
            if (bActivated != C.bActivated)
                return true;
            if (isSummoned != C.isSummoned)
                return true;
            if (faction != C.faction)
                return true;
            return (memcmp(Effects,C.Effects,CARD_ABILITIES_MAX * sizeof(UCHAR)) != 0);
        }
        bool PlayedCard::operator<(const PlayedCard &C) const
        {
            if (OriginalCard != C.OriginalCard) // better to compare ID's, but this should also work
                return (OriginalCard < C.OriginalCard);
            if (Wait != C.Wait)
                return (Wait < C.Wait);
            if (Attack != C.Attack)
                return (Attack < C.Attack);
            if (Health != C.Health)
                return (Health < C.Health);
            if (bPlayed != C.bPlayed)
                return (bPlayed < C.bPlayed);
            if (bActivated != C.bActivated)
                return (bActivated < C.bActivated);
            if (isSummoned != C.isSummoned)
                return (isSummoned < C.isSummoned);
            if (faction != C.faction)
                return (faction < C.faction);
            int mr = memcmp(Effects,C.Effects,CARD_ABILITIES_MAX * sizeof(UCHAR)) < 0;
            return (mr < 0) && (mr != 0);
        }
        PlayedCard& PlayedCard::operator=(const Card *card)
        {
            assertX(card);
            OriginalCard = card;
            Attack = card->GetAttack();
            Health = card->GetHealth();
            Wait = card->GetWait();
            faction = card->GetFaction();
            bQuestSplit = false;
            bPlayed = false;
            bActivated = false;
            isSummoned = false;
            memset(Effects,0,CARD_ABILITIES_MAX);
            fsDmgDealt = 0;
            fsDmgMitigated = 0;
            fsAvoided = 0;
            fsHealed = 0;
            fsSpecial = 0;
            fsOverkill = 0;
            fsDeaths = 0;
            SkillProcBuffer = NULL;
            DeathEvents = 0;
            //uniqueId = nextUniqueId++;
            assertX(false);
            return *this;
        }
        PlayedCard::PlayedCard(const Card *card)
        : uniqueId(nextUniqueId++)
        {
            assertX(card);
            OriginalCard = card;
            Attack = card->GetAttack();
            Health = card->GetHealth();
            Wait = card->GetWait();
            faction = card->GetFaction();
            bQuestSplit = false;
            bPlayed = false;
            bActivated = false;
            isSummoned = false;
            memset(Effects,0,sizeof(Effects));
            fsDmgDealt = 0;
            fsDmgMitigated = 0;
            fsAvoided = 0;
            fsHealed = 0;
            fsSpecial = 0;
            fsOverkill = 0;
            fsDeaths = 0;
            SkillProcBuffer = NULL;
            DeathEvents = 0;
        }
        const UINT PlayedCard::GetId() const { return OriginalCard->GetId(); }
        const UCHAR PlayedCard::GetNativeAttack() const
        {
            return Attack;
        }
        const UCHAR PlayedCard::GetAttack() const
        {
            char atk = Attack - Effects[ACTIVATION_WEAKEN] + Effects[ACTIVATION_RALLY];
            //printf("[%X] %d = %d - %d + %d\n",this,atk,Attack,Effects[ACTIVATION_WEAKEN],Effects[ACTIVATION_RALLY]);
            if (atk > 0)
                return (UCHAR)atk;
            else
                return 0;
        }
        const UCHAR PlayedCard::GetHealth() const { return Health; }
        const bool PlayedCard::GetIsSummoned() const { return isSummoned; }
        const UCHAR PlayedCard::GetMaxHealth() const { return OriginalCard->GetHealth(); }
        const Faction PlayedCard::GetFaction() const { return this->faction; }
        const UCHAR PlayedCard::GetWait() const { return Wait; }
        const CardType PlayedCard::GetType() const
        {
            // this is crap ! - must remove and check the code
            if (!OriginalCard)
                return TYPE_NONE;
            return OriginalCard->GetType();
        }
        const UCHAR PlayedCard::GetEffect(const UCHAR id) const {
            return Effects[id];
        }
        const EFFECT_ARGUMENT PlayedCard::GetAbility(const UCHAR id) const
        {
            // this is crap ! - must remove and check the code
            if (!OriginalCard) {
                assertX(false);
                return 0;
            }
            return this->OriginalCard->GetAbility(id);
        }
        const TargetsCount PlayedCard::GetTargetCount(const UCHAR id) const { return OriginalCard->GetTargetCount(id); }
        const Faction PlayedCard::GetTargetFaction(const UCHAR id) const { return OriginalCard->GetTargetFaction(id); }
        const UCHAR PlayedCard::GetAbilityEvent(const UCHAR id) const { return OriginalCard->GetAbilityEvent(id); }
        //const bool PlayedCard::GetPlayed() const { return bPlayed; }
        //void PlayedCard::Played() { bPlayed = true; }
        //void PlayedCard::ResetPlayedFlag() { bPlayed = false; }
        void PlayedCard::SetAttack(const UCHAR attack) { Attack = attack; }
        void PlayedCard::SetEffect(const UCHAR id, const UCHAR value) { Effects[id] = value; }
        void PlayedCard::SetHealth(const UCHAR health) { Health = health; }
        void PlayedCard::SetIsSummoned(const bool summoned) { isSummoned = summoned; }
        void PlayedCard::Augment(const EFFECT_ARGUMENT amount)
        {
            Effects[ACTIVATION_AUGMENT] += amount;
            //Attack += amount;
        }
        void PlayedCard::Rally(const EFFECT_ARGUMENT amount)
        {
            if (!(this->IsSundered())) {
                Effects[ACTIVATION_RALLY] += amount;
                //Attack += amount;
            }
        }
        EFFECT_ARGUMENT PlayedCard::Weaken(const EFFECT_ARGUMENT amount)
        {
            Effects[ACTIVATION_WEAKEN] += amount;
            //if (amount > Attack) Attack = 0; else Attack -= amount;
            return amount; // this is correct and incorrect at the same time ;(
        }
        void PlayedCard::Berserk(const EFFECT_ARGUMENT amount)
        {
            if (!(this->IsSundered())) {
                Attack += amount;
            }
        }
        void PlayedCard::Protect(const EFFECT_ARGUMENT amount)
        {
            //if (amount > Effects[ACTIVATION_PROTECT])
            Effects[ACTIVATION_PROTECT] += amount;
        }
        bool PlayedCard::Rush(const EFFECT_ARGUMENT amount)
        {
            if (Wait > 0)
            {
                if (amount >= Wait)
                    Wait = 0;
                else
                    Wait -= amount;
                return true;
            }
            else
                return false;
        }
        
        bool PlayedCard::IsDiseased() const	{	return Effects[DMGDEPENDANT_DISEASE] > 0; }

        bool PlayedCard::IsSundered() const	{	return Effects[DMGDEPENDANT_SUNDER] > 0; }

        /**
         *
         * @param amount amount to heal
         * @param QuestEffectId the quest effect we have, only relevant is "Invigorate"
         * @return the actual amount healed
         */
        UCHAR PlayedCard::Heal(EFFECT_ARGUMENT amount,BattleGroundEffect QuestEffectId)
        {
            if(IsDiseased()) {
                return 0;
            }
            
            if (IsDiseased()) return 0;
            if (Health + amount >  OriginalCard->GetHealth()) {
                amount = (OriginalCard->GetHealth() - Health);
                Health =  OriginalCard->GetHealth();
            } else {
                Health += amount;
            }
            // If we healed something and we have invigorate, increase attack
            if (amount > 0 && (QuestEffectId == BattleGroundEffect::invigorate)) {
                if (!(this->IsSundered())) {
                    this->Attack += amount;
                }
            }
            return amount;
        }
        const Card * PlayedCard::GetOriginalCard() const { return OriginalCard; }

        PlayedCard::PlayedCard()
        : uniqueId(nextUniqueId++)
        {
            Attack = 0;
            Health = 0;
            Wait = 0;
            faction = FACTION_NONE;
            bQuestSplit = false;
            bPlayed = false;
            bActivated = false;
            isSummoned = false;
            OriginalCard = 0;
            memset(Effects,0,CARD_ABILITIES_MAX);
            fsDmgDealt = 0;
            fsDmgMitigated = 0;
            fsAvoided = 0;
            fsHealed = 0;
            fsSpecial = 0;
            fsOverkill = 0;
            fsDeaths = 0;
            SkillProcBuffer = NULL;
            DeathEvents = 0;
        }

        /**
         * Copy constructor.
         */
        PlayedCard::PlayedCard(PlayedCard const & original)
        : OriginalCard(original.OriginalCard)
        , Attack(original.Attack)
        , Health(original.Health)
        , Wait(original.Wait)
        , faction(original.faction)
        , bPlayed(original.bPlayed)
        , bActivated(original.bActivated)
        , isSummoned(original.isSummoned)
        , bQuestSplit(original.bQuestSplit)
        , DeathEvents(original.DeathEvents)
        , fsDmgDealt(original.fsDmgDealt)
        , fsDmgMitigated(original.fsDmgMitigated)
        , fsAvoided(original.fsAvoided)
        , fsHealed(original.fsHealed)
        , fsSpecial(original.fsSpecial)
        , fsOverkill(original.fsOverkill)
        , fsDeaths(original.fsDeaths)
        , uniqueId(original.uniqueId)
        {
            memcpy(Effects,original.Effects,sizeof(Effects));
            SkillProcBuffer = original.SkillProcBuffer;
        }

        unsigned int PlayedCard::nextUniqueId(0);

        void
        PlayedCard::setStage(CardActionStages stage)
        {
            this->stage = stage;
        }
        CardActionStages
        PlayedCard::getStage() const
        {
            return this->stage;
        }

    }
}
