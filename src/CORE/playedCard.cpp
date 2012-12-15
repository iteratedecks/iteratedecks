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

        void PlayedCard::DecWait()
        {
            if ((Wait > 0) && (Effects[ACTIVATION_FREEZE] == 0)) {
                Wait--;
            }
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
                        if (Effects[ACTIVATION_JAM])
                            ss << " Jammed";
                        if (Effects[ACTIVATION_FREEZE])
                            ss << " Frozen";
                        if (Effects[DMGDEPENDANT_IMMOBILIZE])
                            ss << " Immobilized";
                        if (Effects[DMGDEPENDANT_DISEASE])
                            ss << " Diseased";
                        if (Effects[ACTIVATION_CHAOS])
                            ss << " Chaosed";
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

            // Assume all cards Blitzing are treated exactly as if they were active;
            // we need to reset this to False after we remove Blitz
            if (bDoBegin && (!bActivated))
                bActivated = true;
            return bDoBegin;
        }
        void PlayedCard::ProcessPoison(BattleGroundEffect QuestEffectId)
        {
            if (IsAlive() && (Effects[DMGDEPENDANT_POISON]))
                SufferDmg(QuestEffectId,Effects[DMGDEPENDANT_POISON]);
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

            if(Effects[SPECIAL_BLITZ]) {
                Effects[SPECIAL_BLITZ] = 0;
                bActivated = false;
            }
        }
        void PlayedCard::Cleanse()
        {
            Effects[DMGDEPENDANT_POISON] = 0;
            Effects[DMGDEPENDANT_DISEASE] = 0;
            Effects[ACTIVATION_JAM] = 0;
            if (bActivated)  // this is bullshit!
                Effects[ACTIVATION_FREEZE] = 0;
            Effects[DMGDEPENDANT_IMMOBILIZE] = 0;
            Effects[ACTIVATION_ENFEEBLE] = 0;
            Effects[ACTIVATION_CHAOS] = 0;
        }
        bool PlayedCard::IsCleanseTarget()
        {
            // Poison, Disease, Jam, Immobilize, Enfeeble, Chaos. (Does not remove Weaken.)
            return (Effects[DMGDEPENDANT_POISON] ||
                    Effects[DMGDEPENDANT_DISEASE] ||
                    Effects[ACTIVATION_JAM] ||
                    (Effects[ACTIVATION_FREEZE] && bActivated) ||
                    Effects[DMGDEPENDANT_IMMOBILIZE] ||
                    Effects[ACTIVATION_ENFEEBLE] ||
                    Effects[ACTIVATION_CHAOS]);
        }
        void PlayedCard::EndTurn()
        {
            Played(); // for rally
        }
        const UCHAR PlayedCard::GetAbilitiesCount() const { return OriginalCard->GetAbilitiesCount(); }
        const UCHAR PlayedCard::GetAbilityInOrder(const UCHAR order) const { return OriginalCard->GetAbilityInOrder(order); }
        void PlayedCard::Infuse(const UCHAR setfaction)
        {
            //OriginalCard.Infuse(setfaction);
            Faction = setfaction;
            SkillProcBuffer[ACTIVATION_INFUSE]++;
        }

        const UCHAR PlayedCard::SufferDmg(BattleGroundEffect QuestEffectId
                                         ,UCHAR const Dmg
                                         ,UCHAR const Pierce
                                         ,UCHAR * const actualdamagedealt
                                         ,UCHAR * SkillProcBuffer
                                         ,UCHAR * OverkillDamage
                                         ,bool bCanRegenerate
                                         ,VLOG *log
                                         ,LOG_RECORD *lr
                                         ,bool * const damageWasDeadly // needed for crush
                                         )
        {
            assertX(OriginalCard);
    // Regeneration happens before the additional strikes from Flurry.
    // Regenerating does not prevent Crush damage
            UCHAR dmg = Dmg;
            UCHAR shield = (UCHAR)GetEffect(ACTIVATION_PROTECT);
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

                // regnerate?
                bool const diseased = this->IsDiseased();
                EFFECT_ARGUMENT const regenerateAmount = this->OriginalCard->GetAbility(DEFENSIVE_REGENERATE);
                bool const hasAbilityRegenerate = (regenerateAmount > 0);
                if (    (!diseased)
                     && (bCanRegenerate)
                     && (hasAbilityRegenerate)
                     && (PROC50)
                   )
                {
                    // This unit regenerates.
                    this->Health = regenerateAmount;
                    fsHealed += regenerateAmount;
                    if (QuestEffectId == BattleGroundEffect::invigorate) {
                        this->Attack += regenerateAmount;
                    }
                    CardSkillProc(DEFENSIVE_REGENERATE);
                    if (lr && log)
                        log->push_back(LOG_RECORD(lr->Target,DEFENSIVE_REGENERATE,Health));
                    // TODO Replace by new logging system, but right now PlayedCards do not know the logger
                    if (bConsoleOutput)
                    {
                        PrintDesc();
                        printf(" regenerated %d health\n",Health);
                    }
                } else {
                    // This unit does not regenerate
                    if (IsAlive()) { // shouldn't die twice ;)
                        fsDeaths++;
                    }
                    this->Health = 0;
                    if (lr && log)
                        log->push_back(LOG_RECORD(lr->Target,0,0)); // death
                    if (bConsoleOutput)
                    {
                        PrintDesc();
                        printf(" died!\n");
                    }
                }
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
                if (actualdamagedealt)
                    *actualdamagedealt = dmg;
                if (bConsoleOutput)
                {
                    PrintDesc();
                    printf(" suffered %d damage\n",dmg);
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
            if (Faction != C.Faction)
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
            if (Faction != C.Faction)
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
            if (Faction != C.Faction)
                return (Faction < C.Faction);
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
            Faction = card->GetFaction();
            bQuestSplit = false;
            bPlayed = false;
            bActivated = false;
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
            Faction = card->GetFaction();
            bQuestSplit = false;
            bPlayed = false;
            bActivated = false;
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
        const UCHAR PlayedCard::GetMaxHealth() const { return OriginalCard->GetHealth(); }
        const UCHAR PlayedCard::GetFaction() const { return Faction; }
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
        const UCHAR PlayedCard::GetTargetCount(const UCHAR id) const { return OriginalCard->GetTargetCount(id); }
        const UCHAR PlayedCard::GetTargetFaction(const UCHAR id) const
        {
            // this is for infuse
            // if card was infused, we gotta force target faction it was infused into
            if ((Faction != OriginalCard->GetFaction()) && (OriginalCard->GetTargetFaction(id) != FACTION_NONE))// check if card was infused and targetfaction is determined
                return Faction; // force faction it was infused into
            else
                return OriginalCard->GetTargetFaction(id);
        }
        const UCHAR PlayedCard::GetAbilityEvent(const UCHAR id) const { return OriginalCard->GetAbilityEvent(id); }
        const bool PlayedCard::GetPlayed() const { return bPlayed; }
        void PlayedCard::Played() { bPlayed = true; }
        void PlayedCard::ResetPlayedFlag() { bPlayed = false; }
        void PlayedCard::SetAttack(const UCHAR attack) { Attack = attack; }
        void PlayedCard::SetEffect(const UCHAR id, const UCHAR value) { Effects[id] = value; }
        void PlayedCard::SetHealth(const UCHAR health) { Health = health; }
        void PlayedCard::Augment(const EFFECT_ARGUMENT amount)
        {
            Effects[ACTIVATION_AUGMENT] += amount;
            //Attack += amount;
        }
        void PlayedCard::Rally(const EFFECT_ARGUMENT amount)
        {
            Effects[ACTIVATION_RALLY] += amount;
            //Attack += amount;
        }
        EFFECT_ARGUMENT PlayedCard::Weaken(const EFFECT_ARGUMENT amount)
        {
            Effects[ACTIVATION_WEAKEN] += amount;
            //if (amount > Attack) Attack = 0; else Attack -= amount;
            return amount; // this is correct and incorrect at the same time ;(
        }
        void PlayedCard::Berserk(const EFFECT_ARGUMENT amount)
        {
            Attack += amount;
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

        /**
         *
         * @param amount amount to heal
         * @param QuestEffectId the quest effect we have, only relevant is "Invigorate"
         * @return the actual amount healed
         */
        UCHAR PlayedCard::Heal(EFFECT_ARGUMENT amount,BattleGroundEffect QuestEffectId)
        {
            assertX(!IsDiseased()); // disallowed
            if (IsDiseased()) return 0;
            if (Health + amount >  OriginalCard->GetHealth()) {
                amount = (OriginalCard->GetHealth() - Health);
                Health =  OriginalCard->GetHealth();
            } else {
                Health += amount;
            }
            // If we healed something and we have invigorate, increase attack
            if (amount && (QuestEffectId == BattleGroundEffect::invigorate)) {
                Attack += amount;
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
            Faction = 0;
            bQuestSplit = false;
            bPlayed = false;
            bActivated = false;
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
        , Faction(original.Faction)
        , bPlayed(original.bPlayed)
        , bActivated(original.bActivated)
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

    }
}
