#include "activeDeck.hpp"

#include "Logger.hpp"
#include "card.hpp"
#include "assert.hpp"
#include "targetManagement.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

#include <cstdlib>
#include <cstring>

// FIXME remove ancient first generation logging code
#include <cstdio>

#include "compat.h"

#include <boost/foreach.hpp>

namespace IterateDecks {
    namespace Core {

        // FIXME remove ancient first generation logging code
        extern bool bConsoleOutput;

        #define BASE64ID	base64ToId // alias
        UINT base64ToId(const unsigned short base64)
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
    // does anyone know if VALOR procs on commander? imagine combo of valor+flurry or valor+fear
    // Balefire shows that it does.
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
                                                              ,false,&overkill,Log
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
            if ((SRC.GetAbility(COMBAT_BURST)) && (target.GetHealth() == target.GetOriginalCard()->GetHealth())) {
                burst = SRC.GetAbility(COMBAT_BURST);
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
                if ((!antiair) && (!SRC.GetAbility(DEFENSIVE_FLYING)) && (PROC50 || (QuestEffectId == BattleGroundEffect::highSkies))) // missed
                {
                    target.fsAvoided += SRC.GetAttack() + valor + burst + target.GetEffect(ACTIVATION_ENFEEBLE); // note that this IGNORES armor and protect
                    Def.SkillProcs[DEFENSIVE_FLYING]++;
                    LOG(this->logger,defensiveAbility(target,SRC,DEFENSIVE_FLYING,1));
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

            // burst "procs" after flying
            if (burst > 0)
            {
                SkillProcs[COMBAT_BURST]++;
                LOG(this->logger,attackBonus(SRC,target,COMBAT_BURST,burst));
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
                    pierce -= armor; // this is for shield
                    armor = 0;
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
                dmg = target.SufferDmg(QuestEffectId
                                      ,dmg
                                      ,pierce
                                      ,&actualDamageDealt
                                      ,0
                                      ,&overkill
                                      ,LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),0,dmg)
                                      ,&damageWasDeadly
                                      );
                LOG(this->logger, cardDamaged(target, SPECIAL_ATTACK, actualDamageDealt));
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
                // crush
                if (SRC.GetAbility(DMGDEPENDANT_CRUSH))
                {
                    UCHAR overkill = 0;
                    if (Def.Commander.HitCommander(QuestEffectId,SRC.GetAbility(DMGDEPENDANT_CRUSH),SRC,Def,*this,true,&overkill,
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
                    LOG(this->logger,abilityOffensive(EVENT_KILL, SRC, DMGDEPENDANT_CRUSH, target, SRC.GetAbility(DMGDEPENDANT_CRUSH)));
                    SkillProcs[DMGDEPENDANT_CRUSH]++;
                }

                ApplyEffects(QuestEffectId,EVENT_KILL,SRC,index,Def,false,false,NULL,0,NULL);
            }

            // Moraku suggests that "on attack" triggers after crush
            Def.ApplyEffects(QuestEffectId,EVENT_ATTACKED,target,targetindex,*this,false,false,NULL,0,&SRC);

            ApplyDefensiveEffects(QuestEffectId, SRC, Def, target, dmg);

            if (dmg > StrongestAttack)
                StrongestAttack = dmg;

            ApplyDamageDependentEffects(QuestEffectId, SRC, Def, target, dmg);

            Def.CheckDeathEvents(target,*this);

            if (!SRC.IsAlive()) // died from counter? during swipe
                return false;
            return true;
        }
    // #############################################################################
        void ActiveDeck::Attack(UINT index, ActiveDeck &Def)
        {
            PlayedCard & attacker = this->getUnitAt(index);

            // Make sure the attacking unit lives.
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
                if(!Def.Commander.IsAlive()) break;

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
                        LOG(this->logger,attackSwipe(attacker)); // swipe "procs" always even if it only hits the unit before it
                        SkillProcs[COMBAT_SWIPE]++;

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
                        if(!Def.Commander.IsAlive()) break;
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
            QuestEffectId = BattleGroundEffect::normal;
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
        void ActiveDeck::SetQuestEffect(BattleGroundEffect EffectId)
        {
            QuestEffectId = EffectId;
        }
        // please note, contructors don't clean up storages, must do it manually and beforehand, even copy constructor
        ActiveDeck::ActiveDeck(const char *DeckHash, const Card *pCDB)
        : pCDB(pCDB)
        , logger(NULL)
        {
            assertX(pCDB);
            assertX(DeckHash);
            QuestEffectId = BattleGroundEffect::normal;
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

            bool isCardOver4000;
            //
            size_t len = strlen(DeckHash);
            //if(len % 2 != 0) {
            //    throw InvalidDeckHashError(InvalidDeckHashError::notEvenChars);
            //}
            //assertX(!(len & 1)); // bytes should go in pairs
            //if (len & 1)
            //    return;
            //len = len >> 1; // div 2
            //Deck.reserve(DEFAULT_DECK_RESERVE_SIZE);

            for (UCHAR i = 0; i < len; i+=2)
            {
                if (DeckHash[i] == '.') break; // delimeter
                if (isspace(DeckHash[i])) break; // not a hash

                if(DeckHash[i] == '-') {
                    i++;
                    isCardOver4000 = true;
                    tid = 4000;
                } else {
                    isCardOver4000 = false;
                    tid = 0;
                }
                assertLT(i + 1u,len); // make sure we have a full hash
                unsigned short cardHash = (DeckHash[i] << 8) + DeckHash[i + 1];
                tid += BASE64ID(cardHash);
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
                    if (tid < 4000 || isCardOver4000)
                    {
                        // this is a card
                        Deck.push_back(&pCDB[tid]);
                        lastid = tid;
                    }
                    else
                    {
                        // this is an encoding for rle
                        for (UINT k = 4000+1; k < tid; k++) // decode RLE, +1 because we already added one card
                            Deck.push_back(&pCDB[lastid]);
                    }
                }
            }
        }
        ActiveDeck::ActiveDeck(const Card *Cmd, Card const * const pCDB)
        : pCDB(pCDB)
        , logger(NULL)
        {
            QuestEffectId = BattleGroundEffect::normal;
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
            if (!Commander.IsDefined()) {
                return false;
            } else if (Deck.empty()) {
                return true;
            } else if (bSoftCheck) {
                return true;
            }
            std::set<UINT> cards;
            bool bLegendary = false;
            for (LCARDS::const_iterator iter = Deck.begin()
                ;iter != Deck.end()
                ;iter++
                )
            {
                UINT rarity = iter->GetRarity();
                if (rarity == RARITY_LEGENDARY) {
                    if (bLegendary) {
                        return false;
                    } else {
                        bLegendary = true;
                    }
                } else if (iter->GetRarity() == RARITY_UNIQUE) {
                    if (cards.find(iter->GetId()) != cards.end()) {
                        return false;
                    } else {
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
        bool ActiveDeck::Evade(PlayedCard *defender, BattleGroundEffect QuestEffectId, bool chaos) {
            return ((defender->GetAbility(DEFENSIVE_EVADE) || (defender->GetType() == TYPE_ASSAULT && QuestEffectId == BattleGroundEffect::quicksilver))
                && (PROC50)
                && (!chaos));
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
                Dest.SkillProcs[DEFENSIVE_INTERCEPT]++;
                LOG(this->logger,defensiveAbility(*targets[destindex-1].first,*targets[destindex].first,DEFENSIVE_INTERCEPT,1));

                destindex = destindex-1;
            }
            else
                if ((destindex < targets.size() - 1) &&
                    (targets[destindex+1].second == targets[destindex].second + 1) &&
                    (targets[destindex+1].first->GetAbility(DEFENSIVE_INTERCEPT) > 0))
                {
                    Dest.SkillProcs[DEFENSIVE_INTERCEPT]++;
                    LOG(this->logger,defensiveAbility(*targets[destindex+1].first,*targets[destindex].first,DEFENSIVE_INTERCEPT,1));

                    destindex = destindex+1;
                }
            return destindex;
        }

        // Will target unit use Payback?
        bool ActiveDeck::Payback(ActiveDeck &targetDeck, PlayedCard *target, PlayedCard &attacker, EVENT_CONDITION EffectType, AbilityId effectId, EFFECT_ARGUMENT effect, bool chaos) {
            if (attacker.IsAlive()
                && EffectType != EVENT_DIED
                && target->GetAbility(DEFENSIVE_PAYBACK)
                && (attacker.GetType() == TYPE_ASSAULT)
                //&& (Src.GetAttack() > 0)
                && (!chaos)
                && PROC50)
            {
                LOG(targetDeck.logger,defensiveAbility(*target,attacker,DEFENSIVE_PAYBACK,1));
                targetDeck.SkillProcs[DEFENSIVE_PAYBACK]++;
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
                LOG(this->logger,abilityTribute(EffectType,*(tributeCard),*(targetCard),aid,effect));
                procDeck->SkillProcs[DEFENSIVE_TRIBUTE]++;
                return true;
            }
            return false;
        }

        void ActiveDeck::ApplyDamageDependentEffects(BattleGroundEffect QuestEffectId,PlayedCard &attacker,ActiveDeck &defenseDeck,PlayedCard &defender,UCHAR dmg) {
            if (dmg > 0 && defender.GetType() == TYPE_ASSAULT)
            {
                // immobilize
                if (attacker.GetAbility(DMGDEPENDANT_IMMOBILIZE)
                    && PROC50)
                {
                    if(!defender.GetEffect(DMGDEPENDANT_IMMOBILIZE)
                        && !defender.GetEffect(ACTIVATION_JAM)
                        && defender.GetWait() <= 1) {
                        defender.SetEffect(DMGDEPENDANT_IMMOBILIZE,attacker.GetAbility(DMGDEPENDANT_IMMOBILIZE));
                        attacker.fsSpecial++; // is it good?
                        LOG(this->logger,abilityOffensive(EVENT_ATTACKED, attacker, DMGDEPENDANT_IMMOBILIZE, defender, 1));
                        SkillProcs[DMGDEPENDANT_IMMOBILIZE]++;
                    }
                }
                // disease
                if (attacker.GetAbility(DMGDEPENDANT_DISEASE)
                    && attacker.GetAbilityEvent(DMGDEPENDANT_DISEASE) == EVENT_EMPTY)
                {
                    if(!defender.GetEffect(DMGDEPENDANT_DISEASE)) {
                        defender.SetEffect(DMGDEPENDANT_DISEASE,attacker.GetAbility(DMGDEPENDANT_DISEASE));
                        attacker.fsSpecial++; // is it good?
                        LOG(this->logger,abilityOffensive(EVENT_EMPTY, attacker, DMGDEPENDANT_DISEASE, defender, 1));
                        SkillProcs[DMGDEPENDANT_DISEASE]++;
                    }
                    //LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DMGDEPENDANT_DISEASE);
                }

                // sunder
                if (attacker.GetAbility(DMGDEPENDANT_SUNDER)
                    && attacker.GetAbilityEvent(DMGDEPENDANT_SUNDER) == EVENT_EMPTY)
                {
                    if(!defender.GetEffect(DMGDEPENDANT_SUNDER)) {
                        defender.SetEffect(DMGDEPENDANT_SUNDER, attacker.GetAbility(DMGDEPENDANT_DISEASE));
                        attacker.fsSpecial++; // is it good?
                        LOG(this->logger,abilityOffensive(EVENT_EMPTY, attacker, DMGDEPENDANT_SUNDER, defender, 1));
                        SkillProcs[DMGDEPENDANT_SUNDER]++;
                    }
                }

                // poison
                if (attacker.GetAbility(DMGDEPENDANT_POISON)
                    && attacker.GetAbilityEvent(DMGDEPENDANT_POISON) == EVENT_EMPTY)
                {
                    if (defender.GetEffect(DMGDEPENDANT_POISON) < attacker.GetAbility(DMGDEPENDANT_POISON)) // overflow
                    {
                        defender.SetEffect(DMGDEPENDANT_POISON,attacker.GetAbility(DMGDEPENDANT_POISON));
                        attacker.fsSpecial += attacker.GetAbility(DMGDEPENDANT_POISON);
                        LOG(this->logger,abilityOffensive(EVENT_EMPTY, attacker, DMGDEPENDANT_POISON, defender, attacker.GetAbility(DMGDEPENDANT_POISON)));
                        SkillProcs[DMGDEPENDANT_POISON]++;
                    }
                }

                // leech
                if (attacker.IsAlive()
                    && attacker.GetAbility(DMGDEPENDANT_LEECH))
                {
                    UCHAR leech = (attacker.GetAbility(DMGDEPENDANT_LEECH) < dmg) ? attacker.GetAbility(DMGDEPENDANT_LEECH) : dmg;
                    if (leech && (!attacker.IsDiseased()))
                    {
                        leech = attacker.Heal(leech,QuestEffectId);
                        attacker.fsHealed += leech;
                        if (leech > 0)
                        {
                            LOG(this->logger,abilityOffensive(EVENT_EMPTY, attacker, DMGDEPENDANT_LEECH, defender, leech));
                            SkillProcs[DMGDEPENDANT_LEECH]++;
                        }
                    }
                }

                // siphon
                if (attacker.GetAbility(DMGDEPENDANT_SIPHON)
                    && defender.GetType() == TYPE_ASSAULT)
                {
                    UCHAR siphon = (attacker.GetAbility(DMGDEPENDANT_SIPHON) < dmg) ? attacker.GetAbility(DMGDEPENDANT_SIPHON) : dmg;
                    if (siphon)
                    {
                        siphon = Commander.Heal(siphon,QuestEffectId);
                        attacker.fsHealed += siphon;
                        if (siphon > 0)
                        {
                            LOG(this->logger,abilityOffensive(EVENT_EMPTY, attacker, DMGDEPENDANT_SIPHON, defender, siphon));
                            SkillProcs[DMGDEPENDANT_SIPHON]++;
                        }
                    }
                }
            }

            // berserk
            if ((dmg > 0)
                && attacker.GetAbility(DMGDEPENDANT_BERSERK)
                && attacker.GetAbilityEvent(DMGDEPENDANT_BERSERK) == EVENT_EMPTY
                && (defender.GetType() != TYPE_STRUCTURE || QuestEffectId != BattleGroundEffect::impenetrable))
            {
                attacker.Berserk(attacker.GetAbility(DMGDEPENDANT_BERSERK));
                LOG(this->logger,abilityOffensive(EVENT_EMPTY, attacker, DMGDEPENDANT_BERSERK, defender, attacker.GetAbility(DMGDEPENDANT_BERSERK)));
                SkillProcs[DMGDEPENDANT_BERSERK]++;
            }
        }

        void ActiveDeck::ApplyDefensiveEffects(BattleGroundEffect QuestEffectId,PlayedCard &attacker,ActiveDeck &defenseDeck,PlayedCard &defender,UCHAR dmg) {
            if(!attacker.IsAlive()) return;

            if ((dmg > 0) && defender.GetAbility(DEFENSIVE_STUN))
            {
                LOG(this->logger,defensiveAbility(defender,attacker,DEFENSIVE_STUN,1));
                defenseDeck.SkillProcs[DEFENSIVE_STUN]++;
                // we just decrement STUN at the end of turn so if we set it to "2" it will skip 1 turn's attacks
                attacker.SetEffect(DEFENSIVE_STUN,2);
            }

            if ((dmg > 0) && defender.GetAbility(DEFENSIVE_COUNTER))
            {
                UCHAR overkill = 0;
                UCHAR cdmg = defender.GetAbility(DEFENSIVE_COUNTER) + attacker.GetEffect(ACTIVATION_ENFEEBLE);
                LOG(this->logger,defensiveAbility(defender,attacker,DEFENSIVE_COUNTER,cdmg));
                defender.fsDmgDealt += attacker.SufferDmg(QuestEffectId,cdmg,0,0,0,&overkill); // counter dmg is enhanced by enfeeble
                defender.fsOverkill += overkill;
                defenseDeck.SkillProcs[DEFENSIVE_COUNTER]++;
                CheckDeathEvents(attacker,defenseDeck);
            }
        }


        void ActiveDeck::applyDamageDependentEffectOnAttack(BattleGroundEffect questEffectId, PlayedCard & src, AbilityId const & abilityId, EFFECT_ARGUMENT const & effectArgument, ActiveDeck & otherDeck, PlayedCard & target) {
            assertX(src.IsDefined());
            assertX(target.IsDefined());
            LOG(this->logger,abilityOffensive(EVENT_ATTACKED, src, abilityId, target, effectArgument));
            switch(abilityId) {
                case DMGDEPENDANT_BERSERK: {
                        if(questEffectId != BattleGroundEffect::impenetrable || target.GetType() != TYPE_STRUCTURE) {
                            src.Berserk(effectArgument);
                            SkillProcs[DMGDEPENDANT_BERSERK]++;
                        }
                    } break;
                case DMGDEPENDANT_CRUSH:
                    throw std::logic_error("crush on attack ... that was not expected");
                case DMGDEPENDANT_DISEASE:
                case DMGDEPENDANT_SUNDER: {
                        assertGT(effectArgument,0u);
                        target.SetEffect(abilityId,effectArgument);
                        src.fsSpecial++;
                        SkillProcs[abilityId]++;
                    } break;
                case DMGDEPENDANT_IMMOBILIZE:
                    throw LogicError("\"immobilize on attack\" not implemented because im lazy");
                case DMGDEPENDANT_LEECH:
                    throw LogicError("leech on attack ... that was not expected");
                case DMGDEPENDANT_POISON:
                    if (target.GetEffect(DMGDEPENDANT_POISON) < effectArgument) { // more than before
                        target.SetEffect(DMGDEPENDANT_POISON,effectArgument);
                        src.fsSpecial += effectArgument;
                        SkillProcs[DMGDEPENDANT_POISON]++;
                    } break;
                default:
                    std::cerr << (UINT)abilityId << std::endl;
                    throw LogicError("something on attack. not implemented because im lazy");
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
            int maxCard = Deck.size();

            if(maxCard > 0) {
                for (int i = maxCard; i > 0; i--) {
                    if(this->getCardAt(i - 1).GetEffect(ACTIVATION_RECHARGE) > 0) {
                        maxCard = i - 1;
                    } else {
                        break;
                    }
                }

                // if all cards are recharged, take the first one
                if (maxCard <= 0) {
                    maxCard = 1;
                }
            }

            if (vi != Deck.end()) // gay ass STL updates !!!
            {
                if (!bOrderMatters)
                {
                    // standard random pick
                    indx = UCHAR(rand() % maxCard);
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
                            indx = UCHAR(rand() % maxCard);
                            //std::clog << "random choice: " << (unsigned int)indx << " thats " << this->getCardAt(indx).toString() << std::endl;
                            // we need to pick first card of a same type, instead of picking this card
                            for (UCHAR i=0;i<maxCard;i++)
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
                if (!indx) {
                    Card const * const c = vi->GetOriginalCard();
                    if (bNormalPick)
                    {
                        if (bDelayFirstCard) {
                            vi->IncWait();
                            bDelayFirstCard = false;
                        }
                        PlayedCard newCard(c);
                        switch (vi->GetType()) {
                            case TYPE_ASSAULT: {
                                    Units.push_back(newCard);
                                } break;
                            case TYPE_STRUCTURE: {
                                    Structures.push_back(newCard);
                                } break;
                            case TYPE_ACTION: {
                                    Actions.push_back(newCard);
                                } break;
                            case TYPE_NONE:
                            case TYPE_COMMANDER:
                                throw LogicError("Card on hand of illegal type");
                        }

                        newCard.SetCardSkillProcBuffer(SkillProcs);
                        LOG(this->logger,cardPlayed(newCard));

                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++) {
                            if (!CardPicks[i])
                            {
                                CardPicks[i] = vi->GetId();
                                break;
                            }
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
                if (QuestEffectId == BattleGroundEffect::decay)
                {
                    Units.back().SetEffect(DMGDEPENDANT_POISON,1);
                    Units.back().SetEffect(DMGDEPENDANT_DISEASE,ABILITY_ENABLED);
                }

                if (QuestEffectId == BattleGroundEffect::poisonAll)
                {
                    Units.back().SetEffect(DMGDEPENDANT_POISON,1);
                }
            }
        }

        void ActiveDeck::CheckDeathEvents(PlayedCard &src, ActiveDeck &EnemyDeck)
        {
            if (src.OnDeathEvent()) {
                ApplyEffects(QuestEffectId,EVENT_DIED,src,-1,EnemyDeck);
                src.Regenerate(QuestEffectId, this->logger);
            }
        }

        void ActiveDeck::AttackDeck(ActiveDeck &Def, bool bSkipCardPicks, unsigned int turn)
        {
            // assume for now that timer is decreased first
            for(LCARDS::iterator iter=Units.begin(); iter != Units.end(); iter++) {
                iter->DecWait();
            }
            for(LCARDS::iterator iter=Structures.begin(); iter != Structures.end(); iter++) {
                iter->DecWait();
            }

            // processing shield, poison and death events all happen in waves, not unit by unit
            for (LCARDS::iterator iter=Units.begin(); iter != Units.end(); iter++) {
                iter->ResetShield(); // according to wiki, shield doesn't affect poison, it wears off before poison procs I believe
            }

            for (LCARDS::iterator iter=Units.begin(); iter != Units.end(); iter++) {
                iter->ProcessPoison(QuestEffectId, this->logger);
            }

            for (LCARDS::iterator iter=Units.begin(); iter != Units.end(); iter++) {
                CheckDeathEvents(*iter, Def);
            }

            // P: The death events may have triggered "on death" abilities,
            //    these may kill another unit.

            // Quest split mark
            if (    (QuestEffectId == BattleGroundEffect::cloneProject)
                 || (    (QuestEffectId == BattleGroundEffect::splitFive)
                      && (turn == 9 || turn == 10)
                    )
               )
            {
                PPCIV GetTo;
                for (LCARDS::iterator vi = Units.begin();vi != Units.end();vi++)
                {
                    if ((vi->IsAlive())
                        && (vi->GetWait() == 0)
                        //&& (!vi->GetEffect(ACTIVATION_JAM))
                        && (!vi->GetEffect(ACTIVATION_FREEZE))
                        //&& (!vi->GetEffect(DMGDEPENDANT_IMMOBILIZE))
                        )
                        GetTo.push_back(PPCARDINDEX(&(*vi),0));
                }
                if (!GetTo.empty())
                {
                    UCHAR rc = UCHAR(rand() % GetTo.size());
                    GetTo[rc].first->SetQuestSplit(true);
                }
            }

            if (!bSkipCardPicks) {
                Card const * const c = this->PickNextCard();
                if (c != NULL) {
                    this->PlayCard(c, Def);
                }
            }

            // P: I have no idea how this fusion code works, let's just
            //    hope it is correct.
            PlayedCard Empty;
            UCHAR iFusionCount = 0;
            for (LCARDS::const_iterator iter = Structures.begin(); iter != Structures.end(); iter++) {
                if (iter->GetAbility(SPECIAL_FUSION) > 0
                    && iter->GetWait() == 0) {
                    iFusionCount++;
                }
            }
            
            // process action cards first
            // P: Actually I cannot imagine a situation with more than
            //    one action card. The only thing would be some
            //    "summon action card" which does not exist.
            for (LCARDS::iterator iter = Actions.begin(); iter != Actions.end(); iter++) {
                // apply actions somehow ...
                ApplyEffects(QuestEffectId, EVENT_EMPTY, *iter, -1, Def);
            }
            for (LCARDS::iterator vi = Actions.begin();vi != Actions.end();vi++) {
                SweepFancyStats(*vi);
            }
            Actions.clear();

            // LEGION; exact order of when this should happen during turn begin is undetermined for now

            for(LCARDS::iterator current = this->Units.begin()
               ;current != this->Units.end()
               ;current++
               )
            {                
                if(current->GetAbility(SPECIAL_LEGION) > 0) {
                    Faction const faction = current->GetFaction();
                    EFFECT_ARGUMENT count = 0;
                    if(current != Units.begin()) {
                        LCARDS::const_iterator prev = current; prev--;
                        if (prev->GetFaction() == faction) {
                            count++;
                        }
                    }

                    LCARDS::const_iterator next = current; next++;
                    if(next != Units.end() && next->GetFaction() == faction) {
                        count++;
                    }
                    assertGE(count,0u);
                    assertLE(count,2u);
                    current->ProcessLegion(count, QuestEffectId);
                }
            }


            // commander card
            // lets work out Infuse:
            // afaik it changes one random card from either of your or enemy deck into bloodthirsty
            // faction plus it changes heal and rally skills' faction, if there were any, into bloodthirsty
            // i believe it can't be mimiced and paybacked (can assume since it's commander skill) and
            // the bad thing about infuse is that we need faction as an attribute of card, we can't pull it out of
            // library, I need to add PlayedCard.Faction, instead of using Card.Faction
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

                    // it can be evaded, according to forums
                    // "his own units that have evade wont ever seem to evade.
                    // (every time ive seen the collossus infuse and as far as i can see, he has no other non bt with evade.)"
                    // so, I assume, evade works for us, but doesn't work for his cards
                    if ((i < defcount) && (t->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == BattleGroundEffect::quicksilver)) && PROC50)
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
                //iter->setStage(CardActionStages::abilitiesDone);
                iter->setStage(CardActionStages::attackDone);
                //iter->Played();
            }}
            // assault cards
            { UINT i = 0;
            for (LCARDS::iterator iter = Units.begin(); iter != Units.end(); iter++,i++) {
                if(!Def.Commander.IsAlive()) break;

                const bool doTurn = iter->BeginTurn();
                assertEQ(iter->getStage(), CardActionStages::before);
                if (doTurn) {
                    ApplyEffects(QuestEffectId,EVENT_EMPTY,*iter,i,Def);
                }
                iter->setStage(CardActionStages::abilitiesDone);

                assertEQ(iter->getStage(), CardActionStages::abilitiesDone);

                // P: I am moving setting the played flag (which is miss-named, better would be 'hasActed')
                //    because there seems to be no reason to do this BEFORE the actual action happens.
                //    If there is a reason for this unintuitive placement it needs to be documented.
                // P: I am redesigning this as it was too coarse. We now use stages.

                if(doTurn) {
                    // tis funny but I need to check Jam for second time in case it was just paybacked
                    if ((!iter->GetEffect(DMGDEPENDANT_IMMOBILIZE))
                    && (!iter->GetEffect(ACTIVATION_JAM))
                    && (!iter->GetEffect(ACTIVATION_FREEZE))
                    && (!iter->GetEffect(DEFENSIVE_STUN)))
                    {
                        if (iter->IsAlive() && iter->GetAttack() > 0 && Def.Commander.IsAlive()) { // can't attack with dead unit ;) also if attack = 0 then dont attack at all
                            Attack(i,Def);
                        }
                    }
                }
                iter->setStage(CardActionStages::attackDone);
            }}
            // refresh commander
            if (Commander.IsDefined() && Commander.IsAlive() && Commander.GetAbility(DEFENSIVE_REFRESH)) { // Bench told refresh procs at the end of player's turn
                EFFECT_ARGUMENT const amountRefreshed = Commander.Refresh(QuestEffectId);
                LOG(this->logger,defensiveRefresh(EVENT_EMPTY,Commander,amountRefreshed));
            }
            // clear dead units here. yours and enemy
            if (!Units.empty()) {
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
                        //vi->ResetPlayedFlag();
                        vi->setStage(CardActionStages::cleanupDone);
                        // P: Why do we reset played flag before clearing enfeeble?
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
                            if (!CardDeaths[i] && !vi->GetIsSummoned())
                            {
                                CardDeaths[i] = vi->GetId();
                                break;
                            }
                        SweepFancyStats(*vi);
                        vi = Structures.erase(vi);
                    }
                    else
                    {
                        // P: I'm pretty sure structures cannot be diseased.
                        if (/*(!vi->IsDiseased()) &&*/ (vi->GetAbility(DEFENSIVE_REFRESH)) && (QuestEffectId != BattleGroundEffect::impenetrable)) {
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
                            if (!Def.CardDeaths[i] && !vi->GetIsSummoned())
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

            // P: I get the feeling that card deaths are ignored here, where do they happen?
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
            if (!bCardPicks)
            {
                for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++)
                {
    #if HASH_SAVES_ORDER
                    ids.push_back(iter->GetId());
    #else
                    ids.insert(iter->GetId());
    #endif
                }
            }
            else
            {
                for (UCHAR i=0;(i<DEFAULT_DECK_RESERVE_SIZE) && (CardPicks[i]);i++)
                {
                    ids.push_back(CardPicks[i]); // multiset is disallowed!
                }
            }

            std::string deckHash;
            UINT cardHash = 0, t;
            unsigned short lastid = 0, cnt = 1;
            if (Commander.IsDefined())
            {
                cardHash = ID2BASE64(Commander.GetId());
                deckHash.append((char*)&cardHash);
                //printf("1: %s -commander\n",(char*)&tmp);
            }

            // TODO: Change to support >4000 ids may not be fully tested; I am not sure if any of this code is used
            unsigned short cardId;
            MSID::iterator si = ids.begin();
            do
            {
                bool isCardOver4000 = *si >= 4000;

                // we can actually use Id range 4000-4095 (CARD_MAX_ID - 0xFFF) for special codes,
                // adding RLE here
                if(isCardOver4000)
                {
                    cardHash = ID2BASE64(*si - 4000);
                }
                else
                {
                    cardHash = ID2BASE64(*si);
                }
                cardId = *si; // save the id and advance the pointer so we can tell if we are at the end of the deck
                si++;
                if ((lastid != cardId) || (si == ids.end()))
                {
                    if (lastid)
                    {
                        t = lastid;
                        if(isCardOver4000) {
                            deckHash += '-';
                        }
                        deckHash.append((char*)&t);
                        if (cnt > 1)
                        {
                            t = ID2BASE64(4000 + cnt); // special code, RLE count
                            deckHash.append((char*)&t);
                        }
                        cnt = 1;
                    }
                    if (si == ids.end())
                    {
                        deckHash.append((char*)&cardHash);
                        //printf("2: %s\n",(char*)&tmp);
                    }
                    lastid = cardId;
                }
                else
                    cnt++;  // RLE, count IDs
            }
            while (si != ids.end());
            return deckHash;
        }

        void
        ActiveDeck::GetTargets(LCARDS &From
                              ,UCHAR TargetFaction
                              ,PPCIV &GetTo
                              ,bool invertFactionCheck
                              )
        {
            if (!invertFactionCheck) {
                GetTo.clear();
            }
            UCHAR pos = 0;
            for (LCARDS::iterator vi = From.begin(); vi != From.end(); vi++) {
                if (    (vi->IsAlive())
                     && (    ((vi->GetFaction() == TargetFaction) && (!invertFactionCheck))
                          || (TargetFaction == FACTION_NONE)
                          || ((vi->GetFaction() != TargetFaction) && (invertFactionCheck))
                        )
                   ) {
                    GetTo.push_back(PPCARDINDEX(&(*vi),pos));
                }
                pos++;
            }
        }

        // skipEffects is an array with a solitary 0 as a terminal
        // targetSkills is an array with a solitary 0 as a terminal
        void ActiveDeck::FilterTargets(PPCIV &targets
                                      ,EFFECT_ARGUMENT const skipEffects[]
                                      ,EFFECT_ARGUMENT const targetSkills[]
                                      ,int const waitMin
                                      ,int const waitMax
                                      ,int const attackLimit
                                      ,bool skipPlayed
                                      )
        {
            PPCIV::iterator vi = targets.begin();
            const EFFECT_ARGUMENT* effect;
            bool erase;
            while (vi != targets.end())
            {
                UCHAR wait = vi->first->GetWait();
                if(vi->first->GetEffect(SPECIAL_BLITZ) > 0) wait = 0;

                erase = false;
                if(waitMax >= 0 && wait > waitMax) {
                    erase = true;
                } else if(waitMin > 0 && wait < waitMin) {
                    // we can ignore a waitMin of 0 since that is the minimum anyway
                    erase = true;
                } else if(attackLimit >= 0 && vi->first->GetAttack() < attackLimit) {
                    erase = true;
                //} else if(skipPlayed && vi->first->GetPlayed()) {                    
                //    erase = true;
                } else if (skipPlayed) {
                    throw Exception("Deprecated code, rewrite using new targetManagement!");
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
        bool PlayedCard::HitCommander(BattleGroundEffect QuestEffectId
                                     , const UCHAR Dmg
                                     , PlayedCard &Src
                                     , ActiveDeck & ownDeck
                                     , ActiveDeck & otherDeck
                                     , bool isCrushDamage
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
                // will a wall *block* the damage?
                if (vi->GetAbility(DEFENSIVE_WALL) && vi->IsAlive())
                {
                    // will the wall *take* the damage?
                    if (QuestEffectId != BattleGroundEffect::impenetrable || isCrushDamage)
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

                        // dmg from crush can't be countered
                        if(!isCrushDamage) {
                            ownDeck.ApplyEffects(QuestEffectId,EVENT_ATTACKED,*vi,index,otherDeck,false,false,NULL,0,&Src);

                            ownDeck.ApplyDefensiveEffects(QuestEffectId, Src, otherDeck, *vi, Dmg);

                            otherDeck.ApplyDamageDependentEffects(QuestEffectId, Src, ownDeck, *vi, Dmg);
                        }
                    }
                    return false;
                }
                index++;
            }

            if(!isCrushDamage) {
                // Commander was attacked, trigger event.
                ownDeck.ApplyEffects(QuestEffectId,EVENT_ATTACKED,*this,0,otherDeck,false,false,NULL,0,&Src);

                ownDeck.ApplyDefensiveEffects(QuestEffectId, Src, otherDeck, *this, Dmg);

                otherDeck.ApplyDamageDependentEffects(QuestEffectId, Src, ownDeck, ownDeck.Commander, Dmg);
            }
            return (SufferDmg(QuestEffectId,Dmg,0,0,0,overkill) > 0);
        }

        bool ActiveDeck::isAlive() const
        {
            return this->Commander.IsAlive();
        }

    }
}
