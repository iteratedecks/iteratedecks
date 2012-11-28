#include "activeDeck.hpp"
#include <iostream>
#include "Logger.hpp"
#include <cstdlib>
#include "card.hpp"
#include "assert.hpp"
#include <cstring>

// FIXME remove ancient first generation logging code
#include <cstdio>

#include "compat.h"

namespace IterateDecks {
    namespace Core {

        // FIXME remove ancient first generation logging code
        extern bool bConsoleOutput;

        #define BASE64ID	BASE642ID // alias
        UINT BASE642ID(const unsigned short base64)
        {
        #define DecodeBase64(x) (((x >= 'A') && (x <= 'Z')) ? (x - 'A') : (((x >= 'a') && (x <= 'z')) ? (x - 'a' + 26) : (((x >= '0') && (x <= '9')) ? (x - '0' + 52) : ((x == '+') ? (62) : (63)))))
        // same stuff as with ID2BASE64, hi and lo swapped
            return DecodeBase64((base64 & 0xFF)) + DecodeBase64((base64 >> 8)) * 64;
        }

        void appendCard(std::stringstream * os
                       ,PlayedCard const & card
                       ,unsigned int const w
                       )
        {
            for(unsigned int i = 0; i < PlayedCard::numberOfCardLines; i++) {
                if(card.IsDefined()) {
                    os[i] << std::setw(w) << card.toRectString(w,i);
                } else {
                    os[i] << std::setw(w) << "";
                }
            }
        }

        void appendCards(std::stringstream * os, LCARDS const & cards, unsigned int const w)
        {
            for(LCARDS::const_iterator iter = cards.begin()
               ;iter != cards.end()
               ;iter++
            ){
                appendCard(os, *iter, w);
            }
        }

        void concatStreams(std::stringstream & os, std::stringstream const * const oss)
        {
            for(unsigned int i = 0; i < PlayedCard::numberOfCardLines; i++) {
                os << oss[i].str() << std::endl;
            }
        }

        void appendCards(std::stringstream & os, LCARDS const & cards, unsigned int const w)
        {
            std::stringstream oss[PlayedCard::numberOfCardLines];
            appendCards(oss,cards,w);
            concatStreams(os,oss);
        }

        PlayedCard & ActiveDeck::getUnitAt(unsigned int const index)
        {
            LCARDS::iterator iter = this->Units.begin();
            for(unsigned int i = 0; i < index; i++) {
                iter++;
            }
            return *iter;
        }

        PlayedCard & ActiveDeck::getActionAt(unsigned int const index)
        {
            LCARDS::iterator iter = this->Actions.begin();
            for(unsigned int i = 0; i < index; i++) {
                iter++;
            }
            return *iter;
        }

        PlayedCard & ActiveDeck::getStructureAt(unsigned int const index)
        {
            LCARDS::iterator iter = this->Structures.begin();
            for(unsigned int i = 0; i < index; i++) {
                iter++;
            }
            return *iter;
        }

        PlayedCard & ActiveDeck::getCardAt(unsigned int const index)
        {
            LCARDS::iterator iter = this->Deck.begin();
            for(unsigned int i = 0; i < index; i++) {
                iter++;
            }
            return *iter;
        }

        PlayedCard const & ActiveDeck::getCardAt(unsigned int const index) const
        {
            LCARDS::const_iterator iter = this->Deck.begin();
            for(unsigned int i = 0; i < index; i++) {
                iter++;
            }
            return *iter;
        }

        void ActiveDeck::Reserve()
        {
        }

        LOG_RECORD* ActiveDeck::LogAdd(LOG_CARD src, UCHAR AbilityID, UCHAR Effect)
        {
            if (!Log) return 0;
            Log->push_back(LOG_RECORD(src,LOG_CARD(),AbilityID,Effect));
            //
            //printf("%d: %d[%d] - %d - = %d\n",src.DeckID,src.DeckID,src.RowID,AbilityID,Effect);
            return &(Log->back());
        }
        LOG_RECORD* ActiveDeck::LogAdd(LOG_CARD src, LOG_CARD dest, UCHAR AbilityID, UCHAR Effect)
        {
            if (!Log) return 0;
            Log->push_back(LOG_RECORD(src,dest,AbilityID,Effect));
            //
            //printf("%d: %d[%d] - %d -> %d: %d[%d] = %d\n",src.DeckID,src.DeckID,src.RowID,dest.DeckID,dest.DeckID,dest.RowID,AbilityID,Effect);
            return &(Log->back());
        }
        UCHAR ActiveDeck::GetAliveUnitsCount()
        {
            UCHAR c = 0;
            for(LCARDS::iterator iter=Units.begin(); iter != Units.end(); iter++) {
                if (iter->IsAlive()) {
                    c++;
                }
            }
            return c;
        }

    // #############################################################################
    // #############################################################################
    // #############################################################################
    // #############################################################################
    // #############################################################################
    // does anyone know if VALOR procs on commander? imagine combo of valor+flurry or valor+fear
    // ok let's assume it does
    #define VALOR_HITS_COMMANDER	true

        void ActiveDeck::AttackCommanderOnce(UCHAR const & index
                                ,PlayedCard & src
                                ,EFFECT_ARGUMENT const & valor
                                ,ActiveDeck & Def
                                ,bool const variant1
                                )
        {
            // can't attack with zero attack, this indicates an error
            assertX(src.GetAttack() > 0);

            if (valor > 0) {
                SkillProcs[COMBAT_VALOR]++;
                LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_VALOR,valor);
            }

            if (src.GetAbility(COMBAT_FEAR) && (Def.Units.size() > index) && Def.getUnitAt(index).IsAlive()) {
                SkillProcs[COMBAT_FEAR]++;
                LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_FEAR);
            }

            UCHAR overkill = 0;
            UCHAR cdmg = src.GetAttack()+valor;
            // Update strongest attack
            if (cdmg > StrongestAttack) {
                StrongestAttack = cdmg;
            }

            // do we hit the commander?
            bool const hitCommander(Def.Commander.HitCommander(QuestEffectId
                                                              ,cdmg
                                                              ,src,Def,*this
                                                              ,true,&overkill,Log
                                                              ,LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index)
                                                                     ,LOG_CARD(Def.LogDeckID,TYPE_COMMANDER,0)
                                                                     ,0,cdmg)
                                                              )
                                   );
            if (hitCommander) {
                DamageToCommander += cdmg;
                FullDamageToCommander += cdmg;
            }
            if(variant1) {
                src.CardSkillProc(SPECIAL_ATTACK); // attack counter
            }

            // gotta check walls & source onDeath here
            for (LCARDS::iterator iter=Def.Structures.begin(); iter != Def.Structures.end(); iter++) {
                Def.CheckDeathEvents(*iter,*this);
            }
            CheckDeathEvents(src,Def);
            src.fsOverkill += overkill;
            src.fsDmgDealt += cdmg;
            // can go berserk after hitting commander too
            if ((src.GetAttack()+valor > 0) && src.GetAbility(DMGDEPENDANT_BERSERK))
            {
                src.Berserk(src.GetAbility(DMGDEPENDANT_BERSERK));
                LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),DMGDEPENDANT_BERSERK,src.GetAbility(DMGDEPENDANT_BERSERK));
                SkillProcs[DMGDEPENDANT_BERSERK]++;
            }
        }
    // #############################################################################
        void ActiveDeck::AttackCommanderOnce1(UCHAR const & index
                                 ,PlayedCard & SRC
                                 ,EFFECT_ARGUMENT const & valor
                                 ,ActiveDeck & Def
                                 )
        {
            AttackCommanderOnce(index, SRC, valor, Def, true);
        }
    // #############################################################################
        void ActiveDeck::AttackCommanderOnce2(UCHAR const & index
                                 ,PlayedCard & SRC
                                 ,EFFECT_ARGUMENT const & valor
                                 ,ActiveDeck & Def
                                 )
        {
            AttackCommanderOnce(index, SRC, valor, Def, false);
        }
    // #############################################################################
        /**
         * @returns true iff we should continue in the swipe loop
         */
        bool ActiveDeck::AttackUnitOrCommanderOnce2(PlayedCard & SRC
                                       ,UCHAR const & index
                                       ,PlayedCard & target
                                       ,UCHAR const & targetindex
                                       ,UCHAR const & swipe
                                       ,UCHAR const & s
                                       ,UCHAR & iSwiped
                                       ,ActiveDeck & Def
                                       )
        {
            bool bGoBerserk = false;
            UCHAR burst = 0;
            if ((SRC.GetAbility(COMBAT_BURST)) && (target.GetHealth() == target.GetOriginalCard()->GetHealth()))
                burst = SRC.GetAbility(COMBAT_BURST);
            if (burst > 0)
            {
                SkillProcs[COMBAT_BURST]++;
                LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_BURST,burst);
                LOG(this->logger,attackBonus(SRC,target,COMBAT_BURST,burst));
            }

            // we should have a valid attaker
            assertX(SRC.IsAlive() && SRC.IsDefined() && SRC.canAttack());

            // after the flurry refactor, this should only be entered with a valid target unless the middle target died during a swipe.

            // if target dies during flurry and slot(s == 1) is aligned to SRC, we deal dmg to commander
            if (    (!target.IsAlive())
                 && ((swipe == 1) || (s == 1))
               )
            {
                if (SRC.GetAttack() > 0) {
                    EFFECT_ARGUMENT valor = (VALOR_HITS_COMMANDER && SRC.GetAbility(COMBAT_VALOR) && (GetAliveUnitsCount() < Def.GetAliveUnitsCount())) ? SRC.GetAbility(COMBAT_VALOR) : 0;
                    assertX(SRC.GetAttack() > 0);
                    AttackCommanderOnce2(index, SRC, valor, Def);
                    // might want to add here check:
                    // if (!Def.Commander.IsAlive()) return;
                    return true;
                } else {
                    return false;
                }
            }
            LOG(this->logger,attackTarget(SRC,target));

            iSwiped++;
            assertX(target.IsAlive()); // must be alive here
            // actual attack
            // must check valor before every attack
            UCHAR valor = (SRC.GetAbility(COMBAT_VALOR) && (GetAliveUnitsCount() < Def.GetAliveUnitsCount())) ? SRC.GetAbility(COMBAT_VALOR) : 0;
            if (valor > 0) {
                SkillProcs[COMBAT_VALOR]++;
                LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_VALOR,valor);
                LOG(this->logger,attackBonus(SRC,target,COMBAT_VALOR,valor));
            }
            // attacking flyer
            UCHAR antiair = SRC.GetAbility(COMBAT_ANTIAIR);
            if (target.GetAbility(DEFENSIVE_FLYING))
            {
                if ((!antiair) && (!SRC.GetAbility(DEFENSIVE_FLYING)) && (PROC50 || (QuestEffectId == QEFFECT_HIGH_SKIES))) // missed
                {
                    target.fsAvoided += SRC.GetAttack() + valor + burst + target.GetEffect(ACTIVATION_ENFEEBLE); // note that this IGNORES armor and protect
                    Def.SkillProcs[DEFENSIVE_FLYING]++;
                    LogAdd(LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DEFENSIVE_FLYING);
                    return true;
                }
            }
            else antiair = 0; // has no effect if target is not flying
            if (antiair > 0)
            {
                SkillProcs[COMBAT_ANTIAIR]++;
                LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_ANTIAIR,antiair);
                LOG(this->logger,attackBonus(SRC,target,COMBAT_ANTIAIR,antiair));
            }
            // enfeeble is taken into account before armor
            UCHAR enfeeble = target.GetEffect(ACTIVATION_ENFEEBLE);
            // now armor & pierce
            UCHAR dmg = SRC.GetAttack() + valor + antiair + enfeeble + burst;
            UCHAR armor = target.GetAbility(DEFENSIVE_ARMORED);
            UCHAR pierce = SRC.GetAbility(COMBAT_PIERCE);
            bool bPierce = false;
            if (armor)
            {
                Def.SkillProcs[DEFENSIVE_ARMORED]++;
                LogAdd(LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DEFENSIVE_ARMORED,armor);
                if (pierce > 0)
                    bPierce = true;
                if (pierce >= armor)
                {
                    armor = 0;
                    pierce -= armor; // this is for shield
                }
                else
                {
                    armor -= pierce;
                    pierce = 0; // this is for shield
                }
                // substract armor from dmg
                if (armor >= dmg)
                {
                    target.fsAvoided += dmg;
                    dmg = 0;
                }
                else
                {
                    target.fsAvoided += armor;
                    dmg -= armor;
                }
            }
            // now we actually deal dmg
            //printf("%s %d = %d => %s %d\n",SRC.GetName(),SRC.GetHealth(),dmg,targets[s]->GetName(),targets[s]->GetHealth());

            //std::clog << "Mark: Before damage dealing" << std::endl;
            bool damageWasDeadly = false;
            if (dmg > 0) {
                UCHAR actualDamageDealt = 0;
                bPierce = bPierce || (target.GetShield() && pierce);
                UCHAR overkill = 0;
                if (bPierce) {
                    LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),COMBAT_PIERCE,pierce);
                }
                dmg = target.SufferDmg(QuestEffectId,dmg, pierce,&actualDamageDealt,0,&overkill,(!SRC.GetAbility(DMGDEPENDANT_DISEASE)),
                    Log,LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),0,dmg),&damageWasDeadly);
                SRC.fsDmgDealt += actualDamageDealt;
                SRC.fsOverkill += overkill;
            }
            if (bPierce) {
                SkillProcs[COMBAT_PIERCE]++;
            }

            //std::clog << "Mark: Before crush check, after regenerate" << std::endl;
            // and now dmg dependant effects
            if (damageWasDeadly) // target just died
            {
                // afaik backfire ignores walls
                if (target.GetAbility(SPECIAL_BACKFIRE))
                {
                    Def.Commander.SufferDmg(QuestEffectId,target.GetAbility(SPECIAL_BACKFIRE));
                    DamageToCommander += SRC.GetAbility(SPECIAL_BACKFIRE);
                    FullDamageToCommander += SRC.GetAbility(SPECIAL_BACKFIRE);
                    Def.SkillProcs[SPECIAL_BACKFIRE]++;
                    LogAdd(LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),LOG_CARD(Def.LogDeckID,TYPE_COMMANDER,0),SPECIAL_BACKFIRE,SRC.GetAbility(SPECIAL_BACKFIRE));
                }
                // crush
                if (SRC.GetAbility(DMGDEPENDANT_CRUSH))
                {
                    UCHAR overkill = 0;
                    if (Def.Commander.HitCommander(QuestEffectId,SRC.GetAbility(DMGDEPENDANT_CRUSH),SRC,Def,*this,false,&overkill,
                        Log,LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_COMMANDER,0),DMGDEPENDANT_CRUSH,SRC.GetAbility(DMGDEPENDANT_CRUSH))))
                    {
                        DamageToCommander += SRC.GetAbility(DMGDEPENDANT_CRUSH);
                        FullDamageToCommander += SRC.GetAbility(DMGDEPENDANT_CRUSH);
                    }

                    // gotta check walls onDeath here
                    for (LCARDS::iterator iter=Def.Structures.begin(); iter != Def.Structures.end(); iter++) {
                        Def.CheckDeathEvents(*iter,*this);
                    }

                    SRC.fsOverkill += overkill;
                    SkillProcs[DMGDEPENDANT_CRUSH]++;
                }
            }

            // Moraku suggests that "on attack" triggers after crush
            Def.ApplyEffects(QuestEffectId,EVENT_ATTACKED,target,targetindex,*this,false,false,NULL,0,&SRC);

            // counter
            if ((dmg > 0) && target.GetAbility(DEFENSIVE_COUNTER))
            {
                UCHAR overkill = 0;
                UCHAR cdmg = target.GetAbility(DEFENSIVE_COUNTER) + SRC.GetEffect(ACTIVATION_ENFEEBLE);
                LOG(this->logger,defensiveAbility(target,SRC,DEFENSIVE_COUNTER,cdmg));
                target.fsDmgDealt += SRC.SufferDmg(QuestEffectId,cdmg,0,0,0,&overkill); // counter dmg is enhanced by enfeeble
                LogAdd(LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),LOG_CARD(LogDeckID,TYPE_ASSAULT,index),DEFENSIVE_COUNTER,cdmg);
                target.fsOverkill += overkill;
                Def.SkillProcs[DEFENSIVE_COUNTER]++;
                CheckDeathEvents(SRC,Def);
            }
            if (dmg > StrongestAttack)
                StrongestAttack = dmg;
            // berserk
            if ((dmg > 0) && SRC.GetAbility(DMGDEPENDANT_BERSERK))
                bGoBerserk = true;
            // if target is dead, we dont need to process this effects
            if (/*targets[s]->IsAlive() && */(dmg > 0))
            {
                // immobilize
                if (SRC.GetAbility(DMGDEPENDANT_IMMOBILIZE) && PROC50)
                {
                    target.SetEffect(DMGDEPENDANT_IMMOBILIZE,SRC.GetAbility(DMGDEPENDANT_IMMOBILIZE));
                    SRC.fsSpecial++; // is it good?
                    SkillProcs[DMGDEPENDANT_IMMOBILIZE]++;
                    LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DMGDEPENDANT_IMMOBILIZE);
                }
                // disease
                if (SRC.GetAbility(DMGDEPENDANT_DISEASE))
                {
                    target.SetEffect(DMGDEPENDANT_DISEASE,SRC.GetAbility(DMGDEPENDANT_DISEASE));
                    SRC.fsSpecial++; // is it good?
                    SkillProcs[DMGDEPENDANT_DISEASE]++;
                    LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DMGDEPENDANT_DISEASE);
                }
                // poison
                if (SRC.GetAbility(DMGDEPENDANT_POISON))
                    if (target.GetEffect(DMGDEPENDANT_POISON) < SRC.GetAbility(DMGDEPENDANT_POISON)) // overflow
                    {
                        target.SetEffect(DMGDEPENDANT_POISON,SRC.GetAbility(DMGDEPENDANT_POISON));
                        SRC.fsSpecial += SRC.GetAbility(DMGDEPENDANT_POISON);
                        SkillProcs[DMGDEPENDANT_POISON]++;
                        LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DMGDEPENDANT_POISON,SRC.GetAbility(DMGDEPENDANT_POISON));
                    }
            }
            // leech
            if (SRC.IsAlive() && SRC.GetAbility(DMGDEPENDANT_LEECH))
            {
                UCHAR leech = (SRC.GetAbility(DMGDEPENDANT_LEECH) < dmg) ? SRC.GetAbility(DMGDEPENDANT_LEECH) : dmg;
                if (leech && (!SRC.IsDiseased()))
                {
                    leech = SRC.Heal(leech,QuestEffectId);
                    SRC.fsHealed += leech;
                    if (leech > 0)
                    {
                        SkillProcs[DMGDEPENDANT_LEECH]++;
                        LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),DMGDEPENDANT_LEECH,leech);
                    }
                }
            }
            // siphon
            if (SRC.GetAbility(DMGDEPENDANT_SIPHON))
            {
                UCHAR siphon = (SRC.GetAbility(DMGDEPENDANT_SIPHON) < dmg) ? SRC.GetAbility(DMGDEPENDANT_SIPHON) : dmg;
                if (siphon)
                {
                    siphon = Commander.Heal(siphon,QuestEffectId);
                    SRC.fsHealed += siphon;
                    if (siphon > 0)
                    {
                        SkillProcs[DMGDEPENDANT_SIPHON]++;
                        LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),DMGDEPENDANT_SIPHON,siphon);
                    }
                }
            }
            if (bGoBerserk)
            {
                SRC.Berserk(SRC.GetAbility(DMGDEPENDANT_BERSERK));
                if (SRC.GetAbility(DMGDEPENDANT_BERSERK))
                {
                    SkillProcs[DMGDEPENDANT_BERSERK]++;
                    LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),DMGDEPENDANT_BERSERK,SRC.GetAbility(DMGDEPENDANT_BERSERK));
                }
            }

            Def.CheckDeathEvents(target,*this);

            if (!SRC.IsAlive()) // died from counter? during swipe
                return false;
            return true;
        }
    // #############################################################################
        void ActiveDeck::Attack(UCHAR index, ActiveDeck &Def)
        {
            PlayedCard & attacker = this->getUnitAt(index);

            // Make sure the attacking unit lives. That should be the case here, thus assertion.
            if(!attacker.IsDefined() || !attacker.IsAlive()) {
                throw std::logic_error("attacking unit is not defined or not alive");
            } else if(!attacker.canAttack()) {
                throw std::logic_error("attacking unit can not attack");
            }

            LOG(this->logger,attackBegin(attacker));

            // Check for flurry, this is independent of whether there is an unit opposing this or not.
            EFFECT_ARGUMENT const flurryBonus(attacker.GetAbility(COMBAT_FLURRY));
            bool const canFlurry(flurryBonus>0);
            bool const doesFlurry(canFlurry && PROC50);
            unsigned int const flurries(doesFlurry ? flurryBonus+1 : 1);
            if (flurries > 1) {
                SkillProcs[COMBAT_FLURRY]++;
                LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_FLURRY,flurries);
                LOG(this->logger,abilityFlurry(attacker,flurries));
            }

            for (UCHAR i=0;i<flurries;i++) {

                // Check whether attacker is still alive. (Could die, get jammed, whatever during flurry/swipe)
                if(!attacker.IsDefined() || !attacker.IsAlive() || !attacker.canAttack()) {
                    break; // leave the flurry loop
                }

                // Is there no unit opposite of the attacking unit, or do we have fear?
                // This condition is inside the flurry loop as this can change... in both directions:
                // - obviously during flurry a unit might die
                // - in theory, after hitting a wall or commander a "summon on attack" or "summon on death" could be triggered
                if (   (index >= (UCHAR)Def.Units.size()) // unit is right of everything opponent has
                    || (!Def.getUnitAt(index).IsAlive()) // opposite unit is dead
                    || (attacker.GetAbility(COMBAT_FEAR)) // unit has fear
                ) {
                    // ... deal DMG to commander directly. BUT STILL PROC50 FLURRY and PROBABLY VALOR
                    bool const canValorCardCommander (VALOR_HITS_COMMANDER && attacker.GetAbility(COMBAT_VALOR));
                    bool const canValorLessUnits (this->GetAliveUnitsCount() < Def.GetAliveUnitsCount());
                    bool const doesValor (canValorCardCommander && canValorLessUnits);
                    EFFECT_ARGUMENT const valor (doesValor ? attacker.GetAbility(COMBAT_VALOR) : 0);
                    assertX(attacker.GetAttack() > 0);
                    AttackCommanderOnce1(index, attacker, valor, Def);
                } else {
                    // ... there is a unit

                    // Now consider swipe. Swipe always procs after flurry
                    // (E.g.: for flurry 2, targets A B C we have ABCABC, and never AABBCC)

                    PlayedCard *targets[3]; //< our potential swipe targets

                    // amount of targets
                    UCHAR const swipe = (attacker.GetAbility(COMBAT_SWIPE)) ? 3 : 1;
                    if (swipe > 1) {
                        // we do swipe
                        if ((index > 0) && (Def.getUnitAt(index-1).IsAlive())) {
                            targets[0] = &Def.getUnitAt(index-1);
                        } else {
                            targets[0] = NULL;
                        }
                        targets[1] = &Def.getUnitAt(index);
                        assertX(targets[1]); // this is aligned to SRC and must be present
                        if ((index+1 < (UCHAR)Def.Units.size()) && (Def.getUnitAt(index+1).IsAlive())) {
                            targets[2] = &Def.getUnitAt(index+1);
                        } else {
                            targets[2] = NULL;
                        }
                        LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_SWIPE);
                        LOG(this->logger,attackSwipe(attacker));
                    } else {
                        // we do not swipe
                        targets[0] = &Def.getUnitAt(index);
                    }

                    // at this point targets[0] to targets[swipe-1] should contain each a valid target to attack

                    UCHAR iSwiped = 0;
                    //
                    attacker.CardSkillProc(SPECIAL_ATTACK); // attack counter
                    // the swipe loop
                    for (UCHAR s=0;s<swipe;s++) {
                        if(!attacker.IsAlive() || !attacker.IsDefined() || !attacker.canAttack()) {
                            // can no longer attack most likely because of
                            // an "on attack" or "on death" ability by a previous swipe target
                            break;
                        }

                        if(s != 1 && targets[s] == NULL) {
                            // If we have left or right target, and that is not defined
                            // we skip this one
                            // (Note that swipe==1 implies targets[0] != NULL)
                            continue;
                        }

                        if (swipe > 1 && (s != 1) && (!targets[s]->IsAlive())) {
                            // If we have left or right target, we do a real swipe and the target is dead
                            // we skip this one
                            continue;
                        }

                        // We either have a target, or we are in the "middle" attack of swipe so we attack the commander/wall
                        UCHAR targetindex = index + (swipe > 1 ? s-1 : 0);

                        std::cout.flush();
                        //std::cerr << Def.toString(true);
                        //std::cerr << this->toString();
                        assertX(attacker.IsAlive() && attacker.IsDefined() && attacker.canAttack());

                        bool doContinue = AttackUnitOrCommanderOnce2(attacker, index, *targets[s], targetindex, swipe,s, iSwiped, Def);
                        if(doContinue) {continue;} else {break;}
                    } // end of swipe

                    // update stats
                    if (iSwiped > 1) {
                        SkillProcs[COMBAT_SWIPE]++;
                    }
                } // end of "not hit commander directly"
            } // end of flurry

            LOG(this->logger,attackEnd(attacker));
        }
    // #############################################################################
    // #############################################################################
    // #############################################################################
    // #############################################################################
    // #############################################################################

        ActiveDeck::ActiveDeck()
        : logger(NULL)
        {
            QuestEffectId = 0;
            Log = 0;
            bOrderMatters = false;
            bDelayFirstCard = false;
            CSIndex = 0;
            CSResult = 0;
            DamageToCommander = 0;
            FullDamageToCommander = 0;
            StrongestAttack = 0;
            memset(SkillProcs,0,sizeof(SkillProcs));
            memset(CardPicks,0,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
            memset(CardDeaths,0,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
            pCDB = NULL;
        }
        ActiveDeck::~ActiveDeck() { Deck.clear(); Units.clear(); Structures.clear(); Actions.clear(); }
        bool ActiveDeck::CheckRequirements(const REQUIREMENT *Reqs)
        {
            if (!Reqs) return true;
            for (UCHAR i=0;(i<REQ_MAX_SIZE) && (Reqs[i].SkillID);i++)
                if (SkillProcs[Reqs[i].SkillID] < Reqs[i].Procs)
                    return false;
            return true;
        }
        bool ActiveDeck::IsAnnihilated()
        {
            if (!Deck.empty()) return false;
            for (LCARDS::iterator vi = Units.begin(); vi != Units.end(); vi++)
                if (vi->IsAlive())
                    return false;
            for (LCARDS::iterator vi = Structures.begin(); vi != Structures.end(); vi++)
                if (vi->IsAlive())
                    return false;
            return true;
        }
        void ActiveDeck::SetFancyStatsBuffer(const UCHAR *resindex, RESULT_BY_CARD *res)
        {
            CSIndex = resindex;
            CSResult = res;
        }
        UCHAR ActiveDeck::GetCountInDeck(UINT Id)
        {
            if (Commander.GetId() == Id)
                return 1;
            UCHAR c = 0;
            for (LCARDS::iterator vi = Deck.begin(); vi != Deck.end(); vi++)
                if (vi->GetId() == Id)
                    c++;
            return c;
        }
        void ActiveDeck::SetQuestEffect(UINT EffectId)
        {
            QuestEffectId = EffectId;
        }
        // please note, contructors don't clean up storages, must do it manually and beforehand, even copy constructor
        ActiveDeck::ActiveDeck(const char *HashBase64, const Card *pCDB)
        : pCDB(pCDB)
        , logger(NULL)
        {
            assertX(pCDB);
            assertX(HashBase64);
            QuestEffectId = 0;
            Log = 0;
            CSIndex = 0;
            CSResult = 0;
            DamageToCommander = 0;
            FullDamageToCommander = 0;
            StrongestAttack = 0;
            bOrderMatters = false;
            bDelayFirstCard = false;
            unsigned short tid = 0, lastid = 0;
            memset(SkillProcs,0,sizeof(SkillProcs));
            memset(CardPicks,0,sizeof(CardPicks));
            memset(CardDeaths,0,sizeof(CardDeaths));
            //
            size_t len = strlen(HashBase64);
            if(len % 2 != 0) {
                throw InvalidDeckHashError(InvalidDeckHashError::notEvenChars);
            }
            assertX(!(len & 1)); // bytes should go in pairs
            if (len & 1)
                return;
            len = len >> 1; // div 2
            //Deck.reserve(DEFAULT_DECK_RESERVE_SIZE);
            for (UCHAR i = 0; i < len; i++)
            {
                if (HashBase64[i << 1] == '.') break; // delimeter
                if (isspace(HashBase64[i << 1])) break; // not a hash
                tid = BASE64ID((HashBase64[i << 1] << 8) + HashBase64[(i << 1) + 1]);
                if (i==0)
                {
                    // first card is commander
                    assertX(tid < CARD_MAX_ID);
                    assertX((tid >= 1000) && (tid < 2000)); // commander Id boundaries
                    Commander = PlayedCard(&pCDB[tid]);
                    Commander.SetCardSkillProcBuffer(SkillProcs);
                }
                else
                {
                    // later cards are not commander
                    assertX(i || (tid < CARD_MAX_ID)); // commander card can't be encoded with RLE
                    if (tid < CARD_MAX_ID)
                    {
                        // this is a card
                        Deck.push_back(&pCDB[tid]);
                        lastid = tid;
                    }
                    else
                    {
                        // this is an encoding for rle
                        for (UINT k = CARD_MAX_ID+1; k < tid; k++) // decode RLE, +1 because we already added one card
                            Deck.push_back(&pCDB[lastid]);
                    }
                }
            }
        }
        ActiveDeck::ActiveDeck(const Card *Cmd, Card const * const pCDB)
        : pCDB(pCDB)
        , logger(NULL)
        {
            QuestEffectId = 0;
            Log = 0;
            bOrderMatters = false;
            bDelayFirstCard = false;
            CSIndex = 0;
            CSResult = 0;
            DamageToCommander = 0;
            FullDamageToCommander = 0;
            StrongestAttack = 0;
            Commander = PlayedCard(Cmd);
            Commander.SetCardSkillProcBuffer(SkillProcs);
            //Deck.reserve(DEFAULT_DECK_RESERVE_SIZE);
            memset(SkillProcs,0,sizeof(SkillProcs));
            memset(CardPicks,0,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
            memset(CardDeaths,0,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
        };
        ActiveDeck::ActiveDeck(const ActiveDeck &D) // need copy constructor
        : pCDB(D.pCDB)
        , logger(D.logger)
        {
            QuestEffectId = D.QuestEffectId;
            Log = 0;
            Commander = D.Commander;
            Commander.SetCardSkillProcBuffer(SkillProcs);
            //Deck.reserve(D.Deck.size());
            Deck = D.Deck;
            //Actions.reserve(D.Actions.size());
            Actions = D.Actions;
            for (LCARDS::iterator iter = Actions.begin(); iter != Actions.end(); iter++) {
                iter->SetCardSkillProcBuffer(SkillProcs); // have to reassign buffers
            }
            //Units.reserve(D.Units.size());
            Units = D.Units;
            for (LCARDS::iterator iter = Units.begin(); iter != Units.end(); iter++) {
                iter->SetCardSkillProcBuffer(SkillProcs); // have to reassign buffers
            }
            //Structures.reserve(D.Structures.size());
            Structures = D.Structures;
            for (LCARDS::iterator iter = Actions.begin(); iter != Actions.end(); iter++) {
                iter->SetCardSkillProcBuffer(SkillProcs); // have to reassign buffers
            }
            bOrderMatters = D.bOrderMatters;
            bDelayFirstCard = D.bDelayFirstCard;
            memcpy(SkillProcs,D.SkillProcs,sizeof(SkillProcs));
            memcpy(CardPicks,D.CardPicks,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
            memcpy(CardDeaths,D.CardDeaths,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
            CSIndex = D.CSIndex;
            CSResult = D.CSResult;
            DamageToCommander = D.DamageToCommander;
            FullDamageToCommander = D.FullDamageToCommander;
            StrongestAttack = D.StrongestAttack;
            if (D.bOrderMatters)
            {
                Hand.clear();
                for (MSID::iterator si=D.Hand.begin();si!=D.Hand.end();si++) {
                    Hand.insert(*si);
                }
            }
            //for (VCARDS::iterator vi = D.Deck.begin();vi != D.Deck.end();vi++)
            //	Deck.push_back(*vi);
        }
        bool ActiveDeck::operator==(const ActiveDeck &D) const
        {
            if (strcmp(GetHash64().c_str(),D.GetHash64().c_str()))
                return false;
            if (Units != D.Units) {
                return false;
            }
            if (Structures != D.Structures) {
                return false;
            }
            if (Actions != D.Actions) {
                return false;
            }
            return true;
        }
        bool ActiveDeck::operator<(const ActiveDeck &D) const
        {
            int sr = strcmp(GetHash64().c_str(),D.GetHash64().c_str());
            if (sr)
                return (sr < 0);
            if (Units.size() != D.Units.size())
                return (Units.size() < D.Units.size());
            for (LCARDS::const_iterator i = Units.begin(), j = D.Units.begin()
                ; i != Units.end()
                ; i++,j++
                ) {
                if(*i != *j) {
                    return *i < *j;
                }
            }
            if (Structures.size() != D.Structures.size())
                return (Structures.size() < D.Structures.size());
            for (LCARDS::const_iterator i = Structures.begin(), j = D.Structures.begin()
                ; i != Structures.end()
                ; i++,j++
                ) {
                if(*i != *j) {
                    return *i < *j;
                }
            }
            if (Actions.size() != D.Actions.size())
                return (Actions.size() < D.Actions.size());
            for (LCARDS::const_iterator i = Actions.begin(), j = D.Actions.begin()
                ; i != Actions.end()
                ; i++,j++
                ) {
                if(*i != *j) {
                    return *i < *j;
                }
            }
            return false;
        }
        const bool ActiveDeck::IsValid(bool bSoftCheck) const
        {
            if (!Commander.IsDefined())
                return false;
            if (Deck.empty())
                return true;
            if (bSoftCheck)
                return true;
            std::set<UINT> cards;
            bool bLegendary = false;
            for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++)
            {
                UINT rarity = iter->GetRarity();
                if (rarity == RARITY_LEGENDARY)
                {
                    if (bLegendary)
                        return false;
                    else
                        bLegendary = true;
                } else {
                    if (iter->GetRarity() == RARITY_UNIQUE)
                    {
                        if (cards.find(iter->GetId()) != cards.end())
                            return false;
                        else
                            cards.insert(iter->GetId());
                    }
                }
            }
            return true;
        }
        void ActiveDeck::SetOrderMatters(const bool bMatters)
        {
            bOrderMatters = bMatters;
        }
        void ActiveDeck::DelayFirstCard()
        {
            bDelayFirstCard = true;
        }
        void ActiveDeck::Add(const Card *c)
        {
            Deck.push_back(c);
        }
        bool ActiveDeck::IsInTargets(PlayedCard *pc, PPCIV *targets) // helper
        {
            for (PPCIV::iterator vi=targets->begin();vi!=targets->end();vi++)
                if (pc == vi->first)
                    return true;
            return false;
        }

        // Will target unit use Evade?
        bool ActiveDeck::Evade(PlayedCard *defender, UINT QuestEffectId, bool chaos) {
            return ((defender->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && (PROC50) && (!chaos));
        }

        UCHAR ActiveDeck::Intercept(PPCIV &targets, UCHAR destindex, ActiveDeck &Dest)
        {
            if (targets.size() < 2)
                return destindex;
            // modify a target by intercept here
            if ((destindex > 0) &&
                (targets[destindex-1].second == targets[destindex].second - 1) &&
                (targets[destindex-1].first->GetAbility(DEFENSIVE_INTERCEPT) > 0))
            {
                destindex = destindex-1;
                Dest.SkillProcs[DEFENSIVE_INTERCEPT]++;
                LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,destindex),DEFENSIVE_INTERCEPT);
            }
            else
                if ((destindex < targets.size() - 1) &&
                    (targets[destindex+1].second == targets[destindex].second + 1) &&
                    (targets[destindex+1].first->GetAbility(DEFENSIVE_INTERCEPT) > 0))
                {
                    destindex = destindex+1;
                    Dest.SkillProcs[DEFENSIVE_INTERCEPT]++;
                    LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,destindex),DEFENSIVE_INTERCEPT);
                }
            return destindex;
        }

        // Will target unit use Payback?
        bool ActiveDeck::Payback(PlayedCard *defender, PlayedCard &Src, ActiveDeck &Dest, EVENT_CONDITION EffectType, AbilityId effectId, EFFECT_ARGUMENT effect, bool chaos) {
            if (Src.IsAlive()
                && EffectType != EVENT_DIED
                && defender->GetAbility(DEFENSIVE_PAYBACK)
                && (Src.GetType() == TYPE_ASSAULT)
                && (Src.GetAttack() > 0)
                && (!chaos)
                && PROC50)
            {
                // TODO there are some complications preventing us from currently performing
                // the payback here; for now just send back whether we did.
                //Src.SetEffect(effectId,effect);
                //defender->fsSpecial += effect;
                //Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
                return true;
            }
            return false;
        }

        // Will Tribute proc?
        // TODO Does not allow for the Effect to be recalculated for things like Heal
        bool ActiveDeck::Tribute(PlayedCard *tributeCard, PlayedCard *targetCard, ActiveDeck *procDeck, EVENT_CONDITION EffectType, AbilityId aid, EFFECT_ARGUMENT effect) {
            if (tributeCard->GetAbility(DEFENSIVE_TRIBUTE)
                && (targetCard->GetType() == TYPE_ASSAULT)
                && (targetCard != tributeCard)
                && PROC50)
            {
                //TODO need to figure out how to get the abstract form of Cleanse() into here
                //procDeck->SkillProcs[DEFENSIVE_TRIBUTE]++;
                //targetCard->Cleanse();
                //LogAdd(lc,DEFENSIVE_TRIBUTE);
                //LogAdd(lc,LOG_CARD(LogDeckID,targetCard->GetType(),SrcPos),aid);
                //vi->first->fsSpecial += effect;
                LOG(this->logger,abilityTribute(EffectType,*(tributeCard),*(targetCard),aid,effect));
                procDeck->SkillProcs[DEFENSIVE_TRIBUTE]++;
                return true;
            }
            return false;
        }

        void ActiveDeck::ApplyEffects(UINT QuestEffectId,EVENT_CONDITION EffectType, PlayedCard &Src,int Position,ActiveDeck &Dest,bool IsMimiced,bool IsFusioned,PlayedCard *Mimicer,UCHAR StructureIndex, PlayedCard * target)
        {
            UCHAR destindex,aid,faction,targetCount;
            PPCIV targets;
            targets.reserve(DEFAULT_DECK_RESERVE_SIZE);
            PPCARDINDEX tmp;
            EFFECT_ARGUMENT effect;
            UCHAR FusionMultiplier = 1;
            if (IsFusioned)
                FusionMultiplier = 2;

            UCHAR SrcPos = StructureIndex;
            if (Position > 0)
                SrcPos = (UCHAR)Position;

            bool bIsSelfMimic = false; // Chaosed Mimic indicator
            if (IsMimiced && Mimicer && (!Units.empty()))
            {
                for (LCARDS::iterator iter = Deck.begin(); iter != Deck.end(); iter++)
                    // FIXME: Intended asssigment? I think not!
                    if (Mimicer = &(*iter))
                    {
                        bIsSelfMimic = true;
                        break;
                    }
            }

            ActiveDeck *procDeck = ((!IsMimiced) || bIsSelfMimic) ? this : &Dest;
            PlayedCard *procCard = (IsMimiced) ? Mimicer : &Src;

            // here is a good question - can paybacked skill be paybacked? - nope
            // can paybacked skill be evaded? - doubt
            // in current model, it can't be, payback applies effect right away, without simulationg it's cast
            // another question is - can paybacked skill be evaded? it is possible, but in this simulator it can't be
            // both here and in branches
            // Mimicked abilities CAN trigger Payback; Confirmed in Tyrant 2.2.67
            UCHAR ac = Src.GetAbilitiesCount();
            //if ((!ac) && (QuestEffectId == QEFFECT_TIME_SURGE) && (!IsMimiced) && (EffectType == EVENT_EMPTY))
            //	ac = 1;

            // TODO probably unnecessary to do all of this quest checking here;
            // should roll it up a level at some point.
            // Also, this could probably be another switch.

            UCHAR questAbilityCount = 0;
            UCHAR questAbilityId = SPECIAL_ATTACK; // TODO need better placeholder
            EFFECT_ARGUMENT questAbilityEffect = 0;
            UCHAR questAbilityTargets = 0;

            if(QuestEffectId == QEFFECT_FRIENDLY_FIRE && EffectType == EVENT_EMPTY) {
                switch(Src.GetType()) {
                case TYPE_COMMANDER: {
                    questAbilityId = ACTIVATION_CHAOS;
                    questAbilityEffect = 1;
                    questAbilityTargets = TARGETSCOUNT_ALL;
                    questAbilityCount++;
                                     } break;
                case TYPE_ASSAULT: {
                    // if the unit already has strike, don't give it to them again
                    if(Src.GetAbility(ACTIVATION_STRIKE) == 0) {
                        questAbilityId = ACTIVATION_STRIKE;
                        questAbilityEffect = 1;
                        questAbilityTargets = TARGETSCOUNT_ONE;
                        questAbilityCount++;
                    }
                                   } break;
                }
            }

            if(QuestEffectId == QEFFECT_GENESIS && EffectType == EVENT_EMPTY) {
                if(Src.GetType() == TYPE_COMMANDER) {
                    questAbilityId = ACTIVATION_SUMMON;
                    // get random assault card; make sure they are not special dev only cards
                    do {
                        questAbilityEffect = UCHAR(rand() % CARD_MAX_ID);
                    } while(questAbilityEffect == 0
                        || ((Card const * const)&pCDB[questAbilityEffect])->GetType() != TYPE_ASSAULT
                        || ((Card const * const)&pCDB[questAbilityEffect])->GetSet() == 0);
                    questAbilityTargets = 0;
                    questAbilityCount++;
                }
            }

            if(QuestEffectId == QEFFECT_TIME_SURGE && EffectType == EVENT_EMPTY) {
                if(Src.GetType() == TYPE_COMMANDER) {
                    questAbilityId = ACTIVATION_RUSH;
                    questAbilityEffect = 1;
                    questAbilityTargets = TARGETSCOUNT_ONE;
                    questAbilityCount++;
                }
            }

            if ((QuestEffectId == QEFFECT_CLONE_PROJECT) && (!IsMimiced) && (Src.GetQuestSplit()) && (EffectType == EVENT_EMPTY))
            {
                Src.SetQuestSplit(false); // remove mark
                questAbilityId = ACTIVATION_SPLIT;
                questAbilityEffect = 1;
                questAbilityTargets = TARGETSCOUNT_ONE;
                questAbilityCount++;
            }

            for (UCHAR aindex=0;aindex<(ac+questAbilityCount);aindex++)
            {
                if (!IsMimiced)
                {
                    if (Src.GetEffect(ACTIVATION_JAM) > 0)
                        break; // card was jammed by payback (or chaos?)
                    if (Src.GetEffect(ACTIVATION_FREEZE) > 0)
                        break; // chaos-mimic-freeze makes this possible
                }

                // Need to check this every time card uses skill because it could be paybacked chaos
                bool chaos = Src.GetEffect(ACTIVATION_CHAOS) != 0;

                if(aindex < ac) {
                    aid = Src.GetAbilityInOrder(aindex);

                    // filter certain types of skills
                    // EMPTY - EMPTY
                    // DIED  - DIED
                    // DIED  - BOTH
                    // PLAY  - PLAY
                    // PLAY  - BOTH
                    EVENT_CONDITION AbilityEventType(Src.GetAbilityEvent(aid));
                    // in general: we only continue if the event type for the skill is the same as the event we process
                    // EMPTY is a special case...
                    if (EffectType == EVENT_EMPTY && AbilityEventType != EVENT_EMPTY) {
                        // ... this is a normal (empty) event handling run, but the card has something different
                        continue;
                    }
                    // ... for non empty we use binary and
                    if (   EffectType != EVENT_EMPTY
                        && ((EffectType & AbilityEventType) == 0)
                        ) {
                            // ... this is a non-normal event handling run, but the card does not have this event
                            continue;
                    }

                    effect = Src.GetAbility(aid); // fusion is applied in the SWTITCH below
                    faction = IsMimiced ? FACTION_NONE : Src.GetTargetFaction(aid);
                    targetCount = Src.GetTargetCount(aid);
                } else {
                    aid = questAbilityId;
                    effect = questAbilityEffect;
                    targetCount = questAbilityTargets;
                    faction = FACTION_NONE;
                }

                switch(aid) {
                case ACTIVATION_CLEANSE:
                    {
                        if (QuestEffectId == QEFFECT_DECAY) { // decay disables all cleansing
                            break;
                        }

                        effect *= FusionMultiplier;
                        assertGT(effect,0u);
                        GetTargets(Units,faction,targets);
                        LOG_CARD lc(LogDeckID,TYPE_ASSAULT,100);

                        PPCIV::iterator vi = targets.begin();
                        while (vi != targets.end())
                        {
                            if (!vi->first->IsCleanseTarget())
                                vi = targets.erase(vi);
                            else vi++;
                        }

                        bool bTributable = IsInTargets(procCard,&targets);

                        RandomizeTarget(targets,targetCount,Dest,false);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (vi = targets.begin();vi != targets.end();vi++)
                        {
                            LOG(this->logger,abilitySupport(EffectType,Src,aid,*(vi->first),effect));

                            vi->first->Cleanse();
                            procCard->fsSpecial += effect;

                            lc.CardID = vi->second;
                            //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                            if(bTributable && Tribute(vi->first, procCard, procDeck, EffectType, aid, effect))
                            {
                                //LOG(this->logger,abilityTribute(EffectType,*(vi->first),Src,aid,effect));
                                //procDeck->SkillProcs[DEFENSIVE_TRIBUTE]++;
                                procCard->Cleanse();
                                //LogAdd(lc,DEFENSIVE_TRIBUTE);
                                //LogAdd(lc,LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),aid);
                                vi->first->fsSpecial += effect;
                            }
                        }
                    } break;

                case ACTIVATION_ENFEEBLE:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);

                        LOG_CARD lc(LogDeckID,TYPE_ASSAULT,100);
                        if (chaos)
                        {
                            GetTargets(Units,faction,targets);
                            lc.DeckID = LogDeckID;
                        }
                        else
                        {
                            GetTargets(Dest.Units,faction,targets);
                            lc.DeckID = Dest.LogDeckID;
                        }

                        RandomizeTarget(targets,targetCount,Dest,!chaos);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                LogAdd(LOG_CARD(Dest.LogDeckID,vi->first->GetType(),vi->second),lc,aid);
                                Dest.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, false));
                                lc.CardID = vi->second;
                                vi->first->SetEffect(aid,vi->first->GetEffect(aid) + effect);

                                procCard->fsSpecial += effect;
                                LogAdd(LOG_CARD(procDeck->LogDeckID,procCard->GetType(),SrcPos),lc,aid,effect);

                                if (Payback(vi->first, Src, Dest, EffectType, ACTIVATION_ENFEEBLE, effect, chaos))  // payback
                                {
                                    Src.SetEffect(aid,Src.GetEffect(aid) + effect);
                                    vi->first->fsSpecial += effect;
                                    Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
                                }
                            }
                    } break;

                case ACTIVATION_HEAL:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);
                        GetTargets(Units,faction,targets);

                        PPCIV::iterator vi = targets.begin();
                        while (vi != targets.end())
                        {
                            if ((vi->first->GetHealth() == vi->first->GetMaxHealth()) || (vi->first->IsDiseased()))
                                vi = targets.erase(vi);
                            else vi++;
                        }

                        // if something tributes this, are we a valid target?
                        bool bTributable = IsInTargets(procCard,&targets);

                        RandomizeTarget(targets,targetCount,Dest,false);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (vi = targets.begin();vi != targets.end();vi++)
                        {
                            LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));

                            procCard->fsHealed += vi->first->Heal(effect,QuestEffectId);
                            //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                            if(bTributable && Tribute(vi->first, procCard, procDeck, EffectType, aid, effect))
                            {
                                //procDeck->SkillProcs[DEFENSIVE_TRIBUTE]++;
                                vi->first->fsHealed += procCard->Heal(effect,QuestEffectId);
                            }
                        }
                    } break;

                case ACTIVATION_SUPPLY:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);
                        if (    (Position >= 0)
                            &&  (    (!IsMimiced)
                                ||   (Mimicer != NULL && (Mimicer->GetType() == TYPE_ASSAULT)) // can only be mimiced by assault cards
                                )
                            )
                        {
                            targets.clear();
                            // If we are not left most, add unit left of us a target
                            if (Position > 0) {
                                targets.push_back(PPCARDINDEX(&(this->getUnitAt(Position-1)),Position-1));
                            }
                            // we are a target
                            targets.push_back(PPCARDINDEX(&(this->getUnitAt(Position)),Position));
                            // if there is a unit right of us, add it as a target
                            if ((DWORD)Position+1 < Units.size()) {
                                targets.push_back(PPCARDINDEX(&(this->getUnitAt(Position+1)),Position+1));
                            }

                            // there should always be a target for supply: self
                            assertX(targets.size() > 0);

                            PPCIV::iterator vi = targets.begin();
                            if (targets.size() > 0)	{
                                // check each target for disease or full health
                                while (vi != targets.end())	{
                                    if ((vi->first->GetHealth() == vi->first->GetMaxHealth())) {
                                        vi = targets.erase(vi);
                                    } else if (vi->first->IsDiseased()) {
                                        // remove diseased targets
                                        LOG(this->logger,abilityFailDisease(EffectType,aid,Src,*(vi->first),IsMimiced,FACTION_NONE,effect));
                                        vi = targets.erase(vi);
                                    } else {
                                        vi++;
                                    }
                                }
                            }

                            // do we still have targets?
                            if (!targets.empty()) {
                                procDeck->SkillProcs[aid]++;
                            } else {
                                // no targets
                                LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,false,FACTION_NONE,effect));
                                break;
                            }

                            //FIXME: That variable is unused, yet has a large right hand side...
                            bool bTributable = IsInTargets(procCard,&targets);

                            // now comes the actual healing
                            for (vi = targets.begin(); vi != targets.end(); vi++) {
                                PlayedCard & target = *(vi->first);
                                assertX(target.IsDefined());
                                LOG(this->logger,abilitySupport(EffectType,Src,aid,target,effect));

                                procCard->fsHealed += vi->first->Heal(effect,QuestEffectId);
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                // tribute
                                bool const targetHasTribute(vi->first->GetAbility(DEFENSIVE_TRIBUTE)>0);
                                if (targetHasTribute) {
                                    if (PROC50) {
                                        LOG(this->logger,abilityTribute(EffectType,*(vi->first),Src,aid,effect));
                                        if ((procCard->GetType() == TYPE_ASSAULT)
                                            && (procCard != vi->first)
                                            && (!procCard->IsDiseased())
                                            ) {
                                                procDeck->SkillProcs[DEFENSIVE_TRIBUTE]++;
                                                vi->first->fsHealed += procCard->Heal(effect,QuestEffectId);
                                        }
                                    } else {
                                        LOG(this->logger,abilityFailNoProc(EffectType,*(vi->first),aid,Src));
                                    }// proc
                                } // tribute
                            }
                        }
                    } break;

                case ACTIVATION_PROTECT:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);

                        GetTargets(Units,faction,targets);

                        RandomizeTarget(targets,targetCount,Dest,false);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        PPCIV::iterator vi = targets.begin();
                        for (vi = targets.begin();vi != targets.end();vi++)
                        {
                            LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                            vi->first->Protect(effect);
                            procCard->fsSpecial += effect;

                            //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                            if(Tribute(vi->first, procCard, procDeck, EffectType, aid, effect))
                            {
                                //LOG(this->logger,abilityTribute(EffectType,*(vi->first),Src,aid,effect));
                                procCard->Protect(effect);
                                vi->first->fsSpecial += effect;
                                //procDeck->SkillProcs[DEFENSIVE_TRIBUTE]++;
                            }
                        }
                    } break;

                case ACTIVATION_JAM:
                    {
                        // infuse is processed on the upper level
                        assertGT(effect,0u);

                        if (chaos)
                            GetTargets(Units,faction,targets);
                        else
                            GetTargets(Dest.Units,faction,targets);

                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_JAM, 0};
                        FilterTargets(targets,skipEffects,NULL,-1,0,-1,false);
                        RandomizeTarget(targets,targetCount,Dest,!chaos);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,(chaos),faction,effect));
                            break;
                        }

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
                        {
                            if (PROC50)
                            {
                                if (Evade(vi->first, QuestEffectId, chaos))
                                {
                                    LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                    Dest.SkillProcs[DEFENSIVE_EVADE]++;
                                }
                                else
                                {
                                    LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                    vi->first->SetEffect(aid,effect);

                                    procCard->fsSpecial += effect;
                                    //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                    procDeck->SkillProcs[aid]++;

                                    if (Payback(vi->first, Src, Dest, EffectType, ACTIVATION_JAM, effect, chaos))  // payback is it 1/2 or 1/4 chance to return jam with payback????
                                    {
                                        Src.SetEffect(aid,effect);
                                        vi->first->fsSpecial += effect;
                                        Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
                                    }
                                }
                            }
                        }
                    } break;

                case ACTIVATION_FREEZE:
                    {
                        assertGT(effect,0u);

                        if (chaos)
                            GetTargets(Units,faction,targets);
                        else
                            GetTargets(Dest.Units,faction,targets);

                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_FREEZE, 0};
                        FilterTargets(targets,skipEffects,NULL,-1,-1,-1,false);
                        RandomizeTarget(targets,targetCount,Dest,!chaos);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                Dest.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                vi->first->SetEffect(aid,effect);
                                procCard->fsSpecial += effect;
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                if (Payback(vi->first, Src, Dest, EffectType, ACTIVATION_FREEZE, effect, chaos))
                                {
                                    Src.SetEffect(aid,effect);
                                    vi->first->fsSpecial += effect;
                                    Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
                                }
                            }
                    } break;

                case ACTIVATION_MIMIC:
                    {
                        // TODO this should be an assert; the check against mimicing mimic should elsewhere
                        if(IsMimiced) { // cannot mimic mimic
                            break;
                        }
                        assertGT(effect,0u);

                        if (chaos || (QuestEffectId == QEFFECT_COPYCAT))
                            GetTargets(Units,faction,targets);
                        else
                            GetTargets(Dest.Units,faction,targets);

                        RandomizeTarget(targets,targetCount,Dest,!chaos);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                Dest.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                if (chaos)
                                    ApplyEffects(QuestEffectId,EVENT_EMPTY,*vi->first,Position,*this,true,false,&Src);
                                else
                                    ApplyEffects(QuestEffectId,EVENT_EMPTY,*vi->first,Position,Dest,true,false,&Src);
                            }
                    } break;

                case ACTIVATION_RALLY:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);

                        GetTargets(Units,faction,targets);

                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_JAM, ACTIVATION_FREEZE, DMGDEPENDANT_IMMOBILIZE, 0};
                        FilterTargets(targets,skipEffects,NULL,-1,0,-1,true);

                        bool bTributable = IsInTargets(procCard,&targets);

                        RandomizeTarget(targets,targetCount,Dest,false);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
                        {
                            LOG(this->logger,abilitySupport(EffectType,Src,aid,*(vi->first),effect));
                            vi->first->Rally(effect);
                            procCard->fsSpecial += effect;
                            //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                            if(bTributable && Tribute(vi->first, procCard, procDeck, EffectType, aid, effect))
                            {
                                //LOG(this->logger,abilityTribute(EffectType,*(vi->first),Src,aid,effect));
                                procCard->Rally(effect);
                                vi->first->fsSpecial += effect;
                                //procDeck->SkillProcs[DEFENSIVE_TRIBUTE]++;
                            }
                        }
                    } break;

                case ACTIVATION_RECHARGE:
                    {
                        if (Src.GetAbility(aid) > 0)
                            if (PROC50)
                                Deck.push_back(Src);
                    } break;

                case ACTIVATION_REPAIR:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);

                        GetTargets(Structures,faction,targets);

                        PPCIV::iterator vi = targets.begin();
                        while (vi != targets.end())
                        {
                            if (vi->first->GetHealth() == vi->first->GetMaxHealth())
                                vi = targets.erase(vi);
                            else vi++;
                        }

                        RandomizeTarget(targets,targetCount,Dest,false);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
                        {
                            vi->first->Heal(effect,QuestEffectId);
                            procCard->fsHealed += effect;
                            //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);
                        }
                    } break;

                case ACTIVATION_SHOCK:
                    {
                        effect *= FusionMultiplier;
                        assertGT(effect,0u);
                        Src.fsDmgDealt += Dest.Commander.SufferDmg(QuestEffectId,effect);
                        DamageToCommander += effect;
                        FullDamageToCommander += effect;
                    } break;

                case ACTIVATION_SIEGE:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);
                        if (chaos)
                            GetTargets(Structures,faction,targets);
                        else
                            GetTargets(Dest.Structures,faction,targets);

                        RandomizeTarget(targets,targetCount,Dest,false);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)

                            if (Evade(vi->first,QuestEffectId,chaos))
                            {
                                vi->first->fsAvoided += effect;
                                Dest.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                UCHAR overkill = 0;
                                UCHAR sdmg = vi->first->SufferDmg(QuestEffectId,effect,0,0,0,&overkill);
                                Dest.CheckDeathEvents(*vi->first,*this);
                                procCard->fsDmgDealt += sdmg;
                                procCard->fsOverkill += overkill;
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);
                            }
                    } break;

                case ACTIVATION_SPLIT:
                    {
                        effect *= FusionMultiplier;
                        assertGT(effect,0u);
                        if (!IsMimiced)
                        {
                            // vectors can be reallocated, lists not, so do it right now
                            // otherwise "on play" effects might happen too late
                            Units.push_back(Src.GetOriginalCard());
                            Units.back().SetCardSkillProcBuffer(SkillProcs);
                            ApplyEffects(QuestEffectId,EVENT_PLAYED,Units.back(),-1,Dest);
                            LOG(this->logger,abilitySupport(EffectType,Src,ACTIVATION_SPLIT,Src,effect));
                        }
                    } break;

                case ACTIVATION_STRIKE:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);

                        if (chaos) {
                            GetTargets(Units,faction,targets);
                        } else {
                            GetTargets(Dest.Units,faction,targets);
                        }

                        RandomizeTarget(targets,targetCount,Dest,!chaos);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        assertX(!targets.empty()); // Targets should never be empty at this point

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++) {
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                // evaded
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                vi->first->fsAvoided += effect;
                                Dest.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                UCHAR overkill = 0;
                                UCHAR sdmg = vi->first->StrikeDmg(QuestEffectId,effect,&overkill);
                                Dest.CheckDeathEvents(*vi->first,*this);
                                procCard->fsDmgDealt += sdmg;
                                procCard->fsOverkill += overkill;
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                if (Payback(vi->first, Src, Dest, EffectType, ACTIVATION_STRIKE, effect, chaos))  // payback
                                {
                                    UCHAR overkill = 0;
                                    vi->first->fsDmgDealt += Src.StrikeDmg(QuestEffectId,effect,&overkill);
                                    CheckDeathEvents(Src,Dest);
                                    vi->first->fsOverkill += overkill;
                                    Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
                                }
                            }
                        }
                    } break;

                case ACTIVATION_SUMMON:
                    {
                        assertGT(effect,0u);
                        assertLT(effect,(UINT)CARD_MAX_ID);
                        assertX(pCDB != NULL);
                        Card const * const summonedCard = &pCDB[effect];
                        if(summonedCard->GetType() == TYPE_ASSAULT) {
                            Units.push_back(summonedCard);
                            LOG(this->logger,abilitySummon(EffectType,Src,Units.back()));
                            Units.back().SetCardSkillProcBuffer(SkillProcs);
                            // TODO this is where the fix for Decay on Summoning needs to happen
                            //ApplyEffects(QuestEffectId,EVENT_PLAYED,Units.back(),-1,Dest);
                        } else if (summonedCard->GetType() == TYPE_STRUCTURE) {
                            Structures.push_back(summonedCard);
                            LOG(this->logger,abilitySummon(EffectType,Src,Structures.back()));
                            Structures.back().SetCardSkillProcBuffer(SkillProcs);
                            //ApplyEffects(QuestEffectId,EVENT_PLAYED,Structures.back(),-1,Dest);
                        } else {
                            std::cerr << "EventCondition=" << (unsigned int)EffectType << " ";
                            std::cerr << "mimic=" << IsMimiced << " mimicer=" << Mimicer << std::endl;
                            std::cerr << "source: " << Src.toString() << " ";
                            std::cerr << "effect argument=" << effect << " ";
                            std::cerr << "card id=" << summonedCard->GetId() << " ";
                            std::cerr << "card name=" << summonedCard->GetName();
                            std::cerr << std::endl;
                            throw LogicError("Summoned something that is neither assault unit nor structure");
                        }
                        PlayCard(summonedCard, Dest);
                    } break;

                case ACTIVATION_WEAKEN:
                    {
                        assertGT(effect,0u);
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);

                        if (chaos)
                            GetTargets(Units,faction,targets);
                        else
                            GetTargets(Dest.Units,faction,targets);

                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_JAM, ACTIVATION_FREEZE, DMGDEPENDANT_IMMOBILIZE, 0};
                        FilterTargets(targets,skipEffects,NULL,-1,0,1,chaos);
                        RandomizeTarget(targets,targetCount,Dest,!chaos);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                Dest.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                UCHAR we = vi->first->Weaken(effect);
                                procCard->fsSpecial += we;
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                if(Payback(vi->first, Src, Dest, EffectType, ACTIVATION_WEAKEN, effect, chaos))
                                {
                                    vi->first->fsSpecial += Src.Weaken(effect);
                                    Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
                                }
                            }
                    } break;

                case ACTIVATION_CHAOS:
                    {
                        assertGT(effect,0u);
                        if (chaos) {
                            GetTargets(Units,faction,targets);
                        } else {
                            GetTargets(Dest.Units,faction,targets);
                        }

                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_JAM, ACTIVATION_FREEZE, ACTIVATION_CHAOS, 0};
                        FilterTargets(targets,skipEffects,NULL,-1,0,1,chaos);
                        RandomizeTarget(targets,targetCount,Dest,!chaos);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                Dest.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                vi->first->SetEffect(ACTIVATION_CHAOS,effect);
                                procCard->fsSpecial += effect;
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                if(Payback(vi->first, Src, Dest, EffectType, ACTIVATION_CHAOS, effect, chaos))
                                {
                                    Src.SetEffect(ACTIVATION_CHAOS, effect);
                                    vi->first->fsSpecial += effect;
                                    Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
                                }
                            }
                    } break;
                case ACTIVATION_RUSH:
                    {
                        assertGT(effect,0u);
                        GetTargets(Units,faction,targets);

                        FilterTargets(targets,NULL,NULL,1,-1,-1,false);

                        RandomizeTarget(targets,targetCount,Dest,false);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
                        {
                            LOG(this->logger,abilityOffensive(EffectType,Src,ACTIVATION_RUSH,*(vi->first),effect));
                            vi->first->Rush(effect);
                            Src.fsSpecial += effect;
                        }
                    } break;
                case SPECIAL_BLITZ:
                    {
                        // TODO can Blitz be Jammed or Freezed?
                        UCHAR targetPos = Position;

                        // TODO move this to a "get opposing card" function
                        UCHAR pos = 0;
                        for (LCARDS::iterator vi = Dest.Units.begin();vi != Dest.Units.end();vi++)
                        {
                            if ((vi->IsAlive())
                                && (pos == targetPos)
                                && (vi->GetWait() == 0)
                                && ((vi->GetFaction() == faction) || (faction == FACTION_NONE))
                                )
                            {
                                Src.SetEffect(aid,effect);
                                LOG(this->logger,abilitySupport(EffectType,Src,aid,Src,effect));
                                targets.push_back(PPCARDINDEX(&(*vi),pos));
                                break;
                            }
                            pos++;
                        }

                        // TODO probably want a more appropriate fail message
                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }
                    } break;
                case ACTIVATION_AUGMENT:
                    {
                        effect *= FusionMultiplier;
                        assertGT(effect,0u);

                        GetTargets(Units,faction,targets);

                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_JAM, ACTIVATION_FREEZE, 0};
                        EFFECT_ARGUMENT targetSkills[] = {ACTIVATION_ENFEEBLE, ACTIVATION_HEAL, ACTIVATION_PROTECT, ACTIVATION_RALLY, ACTIVATION_REPAIR, ACTIVATION_SIEGE, ACTIVATION_STRIKE, ACTIVATION_SUPPLY, ACTIVATION_WEAKEN, 0};
                        FilterTargets(targets,skipEffects,targetSkills,-1,0,-1,true);

                        bool bTributable = IsInTargets(procCard,&targets);

                        RandomizeTarget(targets,targetCount,Dest,false);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
                        {
                            LOG(this->logger,abilitySupport(EffectType,Src,aid,*(vi->first),effect));
                            vi->first->Augment(effect);
                            procCard->fsSpecial += effect;
                            //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                            if(bTributable && Tribute(vi->first, procCard, procDeck, EffectType, aid, effect))
                            {
                                //LOG(this->logger,abilityTribute(EffectType,*(vi->first),Src,aid,effect));
                                procCard->Augment(effect);
                                vi->first->fsSpecial += effect;
                                //procDeck->SkillProcs[DEFENSIVE_TRIBUTE]++;
                            }
                        }
                    } break;
                default:
                    // TODO "on attack" stuff needs to be done for damage dependent
                    if (EffectType == EVENT_ATTACKED) {
                        assertX(Src.IsDefined());
                        assertX(target->IsDefined());
                        assertX(!IsMimiced);
                        assertX(!IsFusioned); // no idea what that is...
                        assertX(target != NULL); // we need a target for dmg dependent stuff
                        EFFECT_ARGUMENT const & effectArgument = Src.GetAbility(aid);
                        applyDamageDependentEffectOnAttack(QuestEffectId, Src, aid, effectArgument, Dest, *target);
                    }
                } // end switch

                targets.clear();

            } // end for(aindex:ac)


        }

        void ActiveDeck::applyDamageDependentEffectOnAttack(UINT questEffectId, PlayedCard & src, AbilityId const & abilityId, EFFECT_ARGUMENT const & effectArgument, ActiveDeck & otherDeck, PlayedCard & target) {
            assertX(src.IsDefined());
            assertX(target.IsDefined());
            LOG(this->logger,abilityOffensive(EVENT_ATTACKED, src, abilityId, target, effectArgument));
            switch(abilityId) {
                case DMGDEPENDANT_BERSERK: {
                        src.Berserk(effectArgument);
                        SkillProcs[DMGDEPENDANT_BERSERK]++;
                    } break;
                case DMGDEPENDANT_CRUSH:
                    throw std::logic_error("crush on attack ... that was not expected");
                case DMGDEPENDANT_DISEASE: {
                        assertGT(effectArgument,0u);
                        target.SetEffect(DMGDEPENDANT_DISEASE,effectArgument);
                        src.fsSpecial++;
                        SkillProcs[DMGDEPENDANT_DISEASE]++;
                    } break;
                case DMGDEPENDANT_IMMOBILIZE:
                    throw std::logic_error("\"immobilize on attack\" not implemented because im lazy");
                case DMGDEPENDANT_LEECH:
                    throw std::logic_error("crush on attack ... that was not expected");
                case DMGDEPENDANT_POISON:
                    if (target.GetEffect(DMGDEPENDANT_POISON) < effectArgument) { // more than before
                        target.SetEffect(DMGDEPENDANT_POISON,effectArgument);
                        src.fsSpecial += effectArgument;
                        SkillProcs[DMGDEPENDANT_POISON]++;
                    } break;
                default:
                    std::cerr << (UINT)abilityId << std::endl;
                    throw std::logic_error("not implemented because im lazy");
            }
        }

        void ActiveDeck::SweepFancyStats(PlayedCard &pc)
        {
            if (!CSIndex) return;
            if (!CSResult) return;
            CSResult[CSIndex[pc.GetId()]].FSRecordCount++;
            CSResult[CSIndex[pc.GetId()]].FSAvoided += pc.fsAvoided;
            CSResult[CSIndex[pc.GetId()]].FSDamage += pc.fsDmgDealt;
            CSResult[CSIndex[pc.GetId()]].FSMitigated += pc.fsDmgMitigated;
            CSResult[CSIndex[pc.GetId()]].FSHealing += pc.fsHealed;
            CSResult[CSIndex[pc.GetId()]].FSSpecial += pc.fsSpecial;
            CSResult[CSIndex[pc.GetId()]].FSOverkill += pc.fsOverkill;
            CSResult[CSIndex[pc.GetId()]].FSDeaths += pc.fsDeaths;
        }
        void ActiveDeck::SweepFancyStatsRemaining()
        {
            if (!CSIndex) return;
            if (!CSResult) return;
            SweepFancyStats(Commander);
            for (LCARDS::iterator vi = Units.begin();vi != Units.end();vi++)
                SweepFancyStats(*vi);
            for (LCARDS::iterator vi = Structures.begin();vi != Structures.end();vi++)
                SweepFancyStats(*vi);
        }
        const Card *ActiveDeck::PickNextCard(bool bNormalPick)
        {
            // pick a random card
            LCARDS::iterator vi = Deck.begin();
            UCHAR indx = 0;
            if (vi != Deck.end()) // gay ass STL updates !!!
            {
                if (!bOrderMatters)
                {
                    // standard random pick
                    indx = UCHAR(rand() % Deck.size());
                }
                else
                {
                    // pick that involves 'hand' and priorities
                    // fill hand
                    UCHAR const handsize(3);
                    if (Deck.size() <= handsize)
                    {
                        // fill hand with all cards in deck
                        Hand.clear();
                        for (UCHAR i=0;i<Deck.size();i++)
                            Hand.insert(i);
                    }
                    else
                    {
                        //std::clog << "pick next card, ordered, Hand.size() = " << Hand.size() << " Deck.size()=" << Deck.size() << std::endl;
                        do
                        {
                            // one random card in deck ...
                            indx = UCHAR(rand() % Deck.size());
                            //std::clog << "random choice: " << (unsigned int)indx << " thats " << this->getCardAt(indx).toString() << std::endl;
                            // we need to pick first card of a same type, instead of picking this card
                            for (UCHAR i=0;i<Deck.size();i++)
                                if (    (this->getCardAt(indx).GetId() == this->getCardAt(i).GetId())
                                     && (Hand.find(indx) == Hand.end())
                                     && (Hand.find(i) == Hand.end())
                                   )
                                {
                                    indx = i;
                                    break;
                                }
                            if (Hand.find(indx) == Hand.end())
                            {
                                Hand.insert(indx);
                                //printf("add to hand %d\n",indx);
                            }
                        }
                        while (Hand.size() < handsize);
                        //std::clog << "Hand is now: ";
                        for(MSID::const_iterator iter = Hand.begin(); iter != Hand.end(); iter++) {
                            unsigned int const i(*iter);
                            PlayedCard const & card(this->getCardAt(i));
                            //std::clog << i << ", ";
                        }
                        //std::clog << std::endl;
                    }
                    indx = (*Hand.begin()); // pick first in set
                    //printf("pick %d\n",indx);
                    MSID tHand;
                    for (MSID::iterator si = Hand.begin(); si != Hand.end(); si++)
                        if (si != Hand.begin()) // this one is picked
                            tHand.insert((*si) - 1); // offset after pick
                    Hand.clear();
                    for (MSID::iterator si = tHand.begin(); si != tHand.end(); si++)
                        Hand.insert(*si);

                    /*for (MSID::iterator si = Hand.begin(); si != Hand.end(); si++)
                    {
                        printf("%d ",*si);
                    }
                    printf(" -> %d\n",indx);*/
                }
            }
            while(vi != Deck.end())
            {
                if (!indx)
                {
                    Card const * const c = vi->GetOriginalCard();
                    if (bNormalPick)
                    {
                        if (bConsoleOutput)
                        {
                            Commander.PrintDesc();
                            printf(" picks ");
                            vi->PrintDesc();
                            printf("\n");
                        }
                        if (bDelayFirstCard)
                        {
                            vi->IncWait();
                            bDelayFirstCard = false;
                        }
                        if (vi->GetType() == TYPE_ASSAULT)
                        {
                            PlayedCard newAssaultCard(c);
                            Units.push_back(newAssaultCard);
                            Units.back().SetCardSkillProcBuffer(SkillProcs);
                        }
                        if (vi->GetType() == TYPE_STRUCTURE)
                        {
                            Structures.push_back(PlayedCard(c));
                            Structures.back().SetCardSkillProcBuffer(SkillProcs);
                        }
                        if (vi->GetType() == TYPE_ACTION)
                        {
                            Actions.push_back(PlayedCard(c));
                            Actions.back().SetCardSkillProcBuffer(SkillProcs);
                        }
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (!CardPicks[i])
                            {
                                CardPicks[i] = vi->GetId();
                                break;
                            }
                        vi = Deck.erase(vi);
                    }
                    return c;
                }
                vi++;
                indx--;
            }
            return 0; // no cards for u
        }

        // TODO eventually want to handle all of the logic of *playing* the card here. PickNextCard
        // should only tell us which one is being played next
        void ActiveDeck::PlayCard(const Card *c, ActiveDeck &Def) {
            if (c->GetType() == TYPE_ACTION)
                ApplyEffects(QuestEffectId,EVENT_PLAYED,Actions.front(),-1,Def);
            else
            if (c->GetType() == TYPE_STRUCTURE)
                ApplyEffects(QuestEffectId,EVENT_PLAYED,Structures.back(),-1,Def);
            else
            if (c->GetType() == TYPE_ASSAULT)
            {
                ApplyEffects(QuestEffectId,EVENT_PLAYED,Units.back(),Units.size() - 1,Def);
                // add quest statuses for decay
                if (QuestEffectId == QEFFECT_DECAY)
                {
                    Units.back().SetEffect(DMGDEPENDANT_POISON,1);
                    Units.back().SetEffect(DMGDEPENDANT_DISEASE,ABILITY_ENABLED);
                }
            }
        }

        void ActiveDeck::CheckDeathEvents(PlayedCard &src, ActiveDeck &Def)
        {
            if (src.OnDeathEvent())
                ApplyEffects(QuestEffectId,EVENT_DIED,src,-1,Def);
        }
        void ActiveDeck::AttackDeck(ActiveDeck &Def, bool bSkipCardPicks)
        {
            // process poison
            for (LCARDS::iterator iter=Units.begin(); iter != Units.end(); iter++) {
                iter->ResetShield(); // according to wiki, shield doesn't affect poison, it wears off before poison procs I believe
                iter->ProcessPoison(QuestEffectId);
            }
            //  Moraku: If �Heal on Death� triggers from poison damage, it will NOT be able to heal another unit dying from poison damage on the same turn. (All poison damage takes place before �On Death� skills trigger)
            for (LCARDS::iterator iter=Units.begin(); iter != Units.end(); iter++) {
                if (iter->OnDeathEvent())
                    ApplyEffects(QuestEffectId,EVENT_DIED,*iter,-1,Def);
            }
            // Quest split mark
            if (QuestEffectId == QEFFECT_CLONE_PROJECT)
            {
                PPCIV GetTo;
                for (LCARDS::iterator vi = Units.begin();vi != Units.end();vi++)
                {
                    if ((vi->IsAlive()) &&
                        (vi->GetWait() == 0) &&
                        (!vi->GetEffect(ACTIVATION_JAM)) && // Jammed
                        (!vi->GetEffect(ACTIVATION_FREEZE)) && // Frozen
                        (!vi->GetEffect(DMGDEPENDANT_IMMOBILIZE)))
                        GetTo.push_back(PPCARDINDEX(&(*vi),0));
                }
                if (!GetTo.empty())
                {
                    UCHAR rc = UCHAR(rand() % GetTo.size());
                    GetTo[rc].first->SetQuestSplit(true);
                }
            }

            if (!bSkipCardPicks)
            {
                const Card *c = PickNextCard();
                if (c)
                {
                    PlayCard(c, Def);
                }
            }

            PlayedCard Empty;
            UCHAR iFusionCount = 0;
            for (LCARDS::const_iterator iter = Structures.begin(); iter != Structures.end(); iter++) {
                if (iter->GetAbility(SPECIAL_FUSION) > 0) {
                    iFusionCount++;
                }
            }
            // action cards
            for (LCARDS::iterator iter = Actions.begin(); iter != Actions.end(); iter++) {
                // apply actions somehow ...
                ApplyEffects(QuestEffectId,EVENT_EMPTY,*iter,-1,Def);
            }
            for (LCARDS::iterator vi = Actions.begin();vi != Actions.end();vi++) {
                SweepFancyStats(*vi);
            }
            Actions.clear();
            // commander card
            // ok lets work out Infuse:
            // infuse - dont know how this works :(
            // ok so afaik it changes one random card from either of your or enemy deck into bloodthirsty
            // faction plus it changes heal and rally skills faction, if there were any, into bloodthirsty
            // i believe it can't be mimiced and paybacked(can assume since it's commander skill) and
            // it can be evaded, according to forums
            // "his own units that have evade wont ever seem to evade.
            // (every time ive seen the collossus infuse and as far as i can see� he has no other non bt with evade.)"
            // so, I assume, evade works for us, but doesn't work for his cards
            // the bad thing about infuse is that we need faction as an attribute of card, we can't pull it out of
            // library, I need to add PlayedCard.Faction, instead of using Card.Faction
            // added
            if (Commander.IsDefined() && Commander.GetAbility(ACTIVATION_INFUSE) > 0)
            {
                // pick a card
                PPCIV targets;
                targets.reserve(DEFAULT_DECK_RESERVE_SIZE);
                GetTargets(Def.Units,FACTION_BLOODTHIRSTY,targets,true);
                UCHAR defcount = (UCHAR)targets.size();
                GetTargets(Units,FACTION_BLOODTHIRSTY,targets,true);
                if (!targets.empty())
                {
                    UCHAR i = UCHAR(rand() % targets.size());
                    i = Intercept(targets, i, Def); // we don't know anything about Infuse being interceptable :( I assume, it is
                    PlayedCard *t = targets[i].first;
                    if ((i < defcount) && (t->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && PROC50) // we check evade only on our cards, enemy cards don't seem to actually evade infuse since it's rather helpful to them then harmful
                    {
                        // evaded infuse
                        //printf("Evaded\n");
                    }
                    else
                        t->Infuse(FACTION_BLOODTHIRSTY);
                }
            }
            // apply actions same way
            if (Commander.IsDefined()) {
                ApplyEffects(QuestEffectId,EVENT_EMPTY,Commander,-1,Def);
            }
            // structure cards
            { UCHAR i = 0;
            for (LCARDS::iterator iter = Structures.begin(); iter != Structures.end(); iter++,i++) {
                // apply actions somehow ...
                if (iter->BeginTurn()) {
                    ApplyEffects(QuestEffectId,EVENT_EMPTY,*iter,-1,Def,false,(iFusionCount >= 3),0,i);
                }
                iter->EndTurn();
            }}
            // assault cards
            { UCHAR i = 0;
            for (LCARDS::iterator iter = Units.begin(); iter != Units.end(); iter++,i++) {
                if (iter->BeginTurn()) {
                    ApplyEffects(QuestEffectId,EVENT_EMPTY,*iter,i,Def);
                    if ((!iter->GetEffect(DMGDEPENDANT_IMMOBILIZE)) && (!iter->GetEffect(ACTIVATION_JAM)) && (!iter->GetEffect(ACTIVATION_FREEZE))) // tis funny but I need to check Jam for second time in case it was just paybacked
                    {
                        if (iter->IsAlive() && iter->GetAttack() > 0 && Def.Commander.IsAlive()) { // can't attack with dead unit ;) also if attack = 0 then dont attack at all
                            Attack(i,Def);
                        }
                    }
                }
                iter->EndTurn();
            }}
            // refresh commander
            if (Commander.IsDefined() && Commander.GetAbility(DEFENSIVE_REFRESH)) { // Bench told refresh procs at the end of player's turn
                EFFECT_ARGUMENT const amountRefreshed = Commander.Refresh(QuestEffectId);
                LOG(this->logger,defensiveRefresh(EVENT_EMPTY,Commander,amountRefreshed));
            }
            // clear dead units here yours and enemy
            if (!Units.empty())
            {
                LCARDS::iterator vi = Units.begin();
                while (vi != Units.end())
                    if (!vi->IsAlive())
                    {
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (!CardDeaths[i])
                            {
                                CardDeaths[i] = vi->GetId();
                                break;
                            }
                        SweepFancyStats(*vi);
                        vi = Units.erase(vi);
                    }
                    else
                    {
                        vi->ResetPlayedFlag();
                        vi->ClearEnfeeble(); // this is important for chaosed skills
                        if ((!vi->IsDiseased()) && (vi->GetAbility(DEFENSIVE_REFRESH))) { // Bench told refresh procs at the end of player's turn
                            EFFECT_ARGUMENT const amountRefreshed = vi->Refresh(QuestEffectId);
                            LOG(this->logger,defensiveRefresh(EVENT_EMPTY,*vi,amountRefreshed));
                        }
                        vi->RemoveDebuffs(); // post-turn
                        vi++;
                    }
            }
            if (!Structures.empty())
            {
                LCARDS::iterator vi = Structures.begin();
                while (vi != Structures.end())
                    if (!vi->IsAlive())
                    {
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (!CardDeaths[i])
                            {
                                CardDeaths[i] = vi->GetId();
                                break;
                            }
                        SweepFancyStats(*vi);
                        vi = Structures.erase(vi);
                    }
                    else
                    {
                        if ((!vi->IsDiseased()) && (vi->GetAbility(DEFENSIVE_REFRESH)) && (QuestEffectId != QEFFECT_IMPENETRABLE)) {// Bench told refresh procs at the end of player's turn
                            EFFECT_ARGUMENT const amountRefreshed = vi->Refresh(QuestEffectId);
                            LOG(this->logger,defensiveRefresh(EVENT_EMPTY,*vi,amountRefreshed));
                        }
                        vi++;
                    }
            }
            //
            if (!Def.Units.empty())
            {
                LCARDS::iterator vi = Def.Units.begin();
                while (vi != Def.Units.end())
                    if (!vi->IsAlive())
                    {
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (!Def.CardDeaths[i])
                            {
                                Def.CardDeaths[i] = vi->GetId();
                                break;
                            }
                        Def.SweepFancyStats(*vi);
                        vi = Def.Units.erase(vi);
                    }
                    else
                    {
                        vi->ClearEnfeeble(); // this is important for chaosed skills
                        vi++;
                    }
            }
            if (!Def.Structures.empty())
            {
                LCARDS::iterator vi = Def.Structures.begin();
                while (vi != Def.Structures.end())
                    if (!vi->IsAlive())
                    {
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (!Def.CardDeaths[i])
                            {
                                Def.CardDeaths[i] = vi->GetId();
                                break;
                            }
                        Def.SweepFancyStats(*vi);
                        vi = Def.Structures.erase(vi);
                    }
                    else
                        vi++;
            }
            // check if delete record from vector via iterator and then browse forward REALLY WORKS????
            // shift cards
        }
        void ActiveDeck::PrintShort()
        {
            std::cout << Commander.GetName() << " [";
            bool first = true;
            for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++) {
                if (first) {
                    first = false;
                } else {
                    std::cout << ",";
                }
                std::cout << iter->GetName();
            }
            std::cout << "]" << std::endl;
        }

        /**
         * Returns a nice string representation of the deck.
         */
        std::string ActiveDeck::toString(bool const & reversed, unsigned int const w)
        {
            std::stringstream ssDeck;
            std::stringstream oss[PlayedCard::numberOfCardLines];

            if(!reversed) {
                appendCards(ssDeck,this->Units,w);
                appendCard(oss,this->Commander,w);
                appendCards(oss,this->Structures,w);
                appendCards(oss,this->Actions,w);
                concatStreams(ssDeck,oss);
            } else {
                appendCard(oss,this->Commander,w);
                appendCards(oss,this->Structures,w);
                appendCards(oss,this->Actions,w);
                concatStreams(ssDeck,oss);
                appendCards(ssDeck,this->Units,w);
            }
            return ssDeck.str();
        }

        std::string ActiveDeck::GetDeck() const
        {
            if (Deck.empty()) {
                return std::string();
            }
            std::string s;
            char buffer[10];
            if (Commander.IsDefined())
            {
                _itoa_s(Commander.GetId(),buffer,10);
                s.append(buffer);
            }
            for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++) {
                if (!s.empty())
                    s.append(",");
                _itoa_s(iter->GetId(),buffer,10);
                s.append(buffer);
            }
            return s;
        }

        std::string ActiveDeck::GetHash64(bool bCardPicks) const
        {
    #define HASH_SAVES_ORDER	1
            if (Deck.empty() && ((!bCardPicks) || (!CardPicks[0])))
                return std::string();
    #if HASH_SAVES_ORDER
            typedef std::vector<UINT> MSID;
    #else
            typedef std::multiset<UINT> MSID; // I <3 sets, they keep stuff sorted ;)
    #endif
            MSID ids;
            if (!bCardPicks) {
                for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++) {
    #if HASH_SAVES_ORDER
                    ids.push_back(iter->GetId());
    #else
                    ids.insert(iter->GetId());
    #endif
                }
            }
            else
                for (UCHAR i=0;(i<DEFAULT_DECK_RESERVE_SIZE) && (CardPicks[i]);i++)
                    ids.push_back(CardPicks[i]); // multiset is disallowed!
            std::string s;
            UINT tmp = 0, t;
            unsigned short lastid = 0, cnt = 1;
            if (Commander.IsDefined())
            {
                tmp = ID2BASE64(Commander.GetId());
                s.append((char*)&tmp);
                //printf("1: %s -commander\n",(char*)&tmp);
            }
            MSID::iterator si = ids.begin();
            do
            {
                // we can actually use Id range 4000-4095 (CARD_MAX_ID - 0xFFF) for special codes,
                // adding RLE here
                tmp = ID2BASE64(*si);
                si++;
                if ((lastid != tmp) || (si == ids.end()))
                {
                    if (lastid)
                    {
                        t = lastid;
                        if (cnt == 2)
                        {
                            s.append((char*)&t);
                            s.append((char*)&t);
                            //printf("4: %s -dupe\n",(char*)&t);
                            //printf("4: %s -dupe\n",(char*)&t);

                        }
                        else
                            if (cnt > 2)
                            {
                                s.append((char*)&t);
                                //printf("3: %s -value\n",(char*)&t);
                                t = ID2BASE64(CARD_MAX_ID + cnt); // special code, RLE count
                                s.append((char*)&t);
                                //printf("3: %s -rle\n",(char*)&t);
                            }
                            else
                            {
                                s.append((char*)&t);
                                //printf("5: %s\n",(char*)&t);
                            }
                        cnt = 1;
                    }
                    if (si == ids.end())
                    {
                        s.append((char*)&tmp);
                        //printf("2: %s\n",(char*)&tmp);
                    }
                    lastid = tmp;
                }
                else
                    cnt++;  // RLE, count IDs
            }
            while (si != ids.end());
            return s;
        }
        void ActiveDeck::GetTargets(LCARDS &From, UCHAR TargetFaction, PPCIV &GetTo, bool bForInfuse)
        {
            if (!bForInfuse)
                GetTo.clear();
            UCHAR pos = 0;
            for (LCARDS::iterator vi = From.begin();vi != From.end();vi++)
            {
                if ((vi->IsAlive()) && (((vi->GetFaction() == TargetFaction) && (!bForInfuse)) || (TargetFaction == FACTION_NONE) || ((vi->GetFaction() != TargetFaction) && (bForInfuse))))
                    GetTo.push_back(PPCARDINDEX(&(*vi),pos));
                pos++;
            }
        }

        // skipEffects is an array with a solitary 0 as a terminal
        // targetSkills is an array with a solitary 0 as a terminal
        void ActiveDeck::FilterTargets(PPCIV &targets, const EFFECT_ARGUMENT skipEffects[], const EFFECT_ARGUMENT targetSkills[], const int waitMin, const int waitMax, const int attackLimit, bool skipPlayed)
        {
            PPCIV::iterator vi = targets.begin();
            const EFFECT_ARGUMENT* effect;
            bool erase;
            while (vi != targets.end())
            {
                erase = false;
                if(waitMax >= 0 && vi->first->GetWait() > waitMax) {
                    erase = true;
                } else if(waitMin > 0 && vi->first->GetWait() < waitMin) {
                    // we can ignore a waitMin of 0 since that is the minimum anyway
                    erase = true;
                } else if(attackLimit >= 0 && vi->first->GetAttack() < attackLimit) {
                    erase = true;
                } else if(skipPlayed && vi->first->GetPlayed()) {
                    erase = true;
                } else {
                    if(skipEffects != NULL) {
                        for (effect = skipEffects; *effect != 0; ++effect) {
                            assertLT(*effect,CARD_ABILITIES_MAX); // make sure someone gave us our terminal
                            if(vi->first->GetEffect(*effect)) {
                                erase = true;
                                break;
                            }
                        }
                    }

                    if(!erase && targetSkills != NULL) {
                        erase = true;
                        for (effect = targetSkills; *effect != 0; ++effect) {
                            assertLT(*effect,CARD_ABILITIES_MAX); // make sure someone gave us our terminal
                            if(vi->first->GetAbility(*effect)
                                && vi->first->GetAbilityEvent(*effect) == EVENT_EMPTY) {
                                erase = false;
                                break;
                            }
                        }
                    }
                }

                if (erase)
                    vi = targets.erase(vi);
                else
                    vi++; // skip
            }
        }

        // Choose targets up to targetCount.
        void ActiveDeck::RandomizeTarget(PPCIV &targets, UCHAR targetCount, ActiveDeck &Dest, bool canIntercept)
        {
            if ((targetCount != TARGETSCOUNT_ALL) && (!targets.empty()))
            {
                UCHAR destindex = UCHAR(rand() % targets.size());
                if (canIntercept) {
                    destindex = Intercept(targets, destindex, Dest);
                }
                PPCARDINDEX tmp = targets[destindex];
                targets.clear();
                targets.push_back(tmp);
            }
        }

        /**
         * Hit the commander.
         */
        bool PlayedCard::HitCommander(UINT QuestEffectId
                                     , const UCHAR Dmg
                                     , PlayedCard &Src
                                     , ActiveDeck & ownDeck
                                     , ActiveDeck & otherDeck
                                     , bool bCanBeCountered
                                     , UCHAR *overkill
                                     , VLOG *log
                                     , LOG_RECORD *lr
                                     )
        {
            LCARDS & Structures(ownDeck.Structures);
            assertX(GetType() == TYPE_COMMANDER); // double check for debug

            // 0 dmg is pointless and indicates an error
            if(Dmg <= 0) {
                throw std::invalid_argument("Zero 0 damage in PlayedCard::HitCommander");
            }

            // find a wall to break it ;)
            UCHAR index = 0;
            for (LCARDS::iterator vi = Structures.begin();vi!=Structures.end();vi++)
            {
                if (vi->GetAbility(DEFENSIVE_WALL) && vi->IsAlive())
                {
                    if (QuestEffectId != QEFFECT_IMPENETRABLE)
                    {
                        vi->CardSkillProc(DEFENSIVE_WALL);
                        if (lr)
                        {
                            lr->Target.CardID = index;
                            lr->Target.RowID = TYPE_STRUCTURE;
                        }



                        // walls can counter and regenerate
                        vi->SufferDmg(QuestEffectId,Dmg,0,0,0,overkill);

                        // probably here wall's "on attacked" skills
                        assertX(vi->IsDefined());
                        assertX(Src.IsDefined());
                        ownDeck.ApplyEffects(QuestEffectId,EVENT_ATTACKED,*vi,index,otherDeck,false,false,NULL,0,&Src);

                        if (vi->GetAbility(DEFENSIVE_COUNTER) && bCanBeCountered) // counter, dmg from crush can't be countered
                        {
                            vi->CardSkillProc(DEFENSIVE_COUNTER);
                            EFFECT_ARGUMENT cdmg = vi->GetAbility(DEFENSIVE_COUNTER) + Src.GetEffect(ACTIVATION_ENFEEBLE);
                            vi->fsDmgDealt += cdmg;
                            UCHAR loverkill = 0;
                            if (lr && log)
                                log->push_back(LOG_RECORD(lr->Target,lr->Src,DEFENSIVE_COUNTER,cdmg));
                            Src.SufferDmg(QuestEffectId,cdmg,0,0,0,&loverkill); // counter dmg is enhanced by enfeeble
                            vi->fsOverkill += loverkill;
                        }
                    }
                    return false;
                }
                index++;
            }

            // Commander was attacked, trigger event.
            ownDeck.ApplyEffects(QuestEffectId,EVENT_ATTACKED,*this,0,otherDeck,false,false,NULL,0,&Src);

            // no walls found then hit commander
            // ugly - counter procs before commander takes dmg, but whatever
            if (GetAbility(DEFENSIVE_COUNTER) && bCanBeCountered) // commander can counter aswell
            {
                CardSkillProc(DEFENSIVE_COUNTER);
                UCHAR loverkill = 0;
                EFFECT_ARGUMENT cdmg = GetAbility(DEFENSIVE_COUNTER) + Src.GetEffect(ACTIVATION_ENFEEBLE);
                if (lr && log)
                    log->push_back(LOG_RECORD(lr->Target,lr->Src,DEFENSIVE_COUNTER,cdmg));
                Src.SufferDmg(QuestEffectId,cdmg,0,0,0,&loverkill); // counter dmg is enhanced by enfeeble
                fsOverkill += loverkill;
            }
            return (SufferDmg(QuestEffectId,Dmg,0,0,0,overkill) > 0);
        }

    }
}
