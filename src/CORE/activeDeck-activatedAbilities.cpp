// this is an additional source file for activeDeck.hpp
// it contains code related to the activated abilities as the main file gets way to large.

#include "activeDeck.hpp"
#include "Logger.hpp"
#include "assert.hpp"
#include "targetManagement.hpp"
#include "card.hpp"
#include <boost/foreach.hpp>
#include <iostream>

namespace IterateDecks {
    namespace Core {
        void
        ActiveDeck::ApplyEffects(BattleGroundEffect QuestEffectId
                                ,EVENT_CONDITION EffectType
                                ,PlayedCard & Src
                                ,int Position
                                ,ActiveDeck & EnemyDeck
                                ,bool IsMimiced
                                ,bool IsFusioned
                                ,PlayedCard * Mimicer
                                ,UCHAR StructureIndex
                                ,PlayedCard * target
                                )
        {
            UCHAR aid;
            TargetsCount targetCount;
            Faction faction, infusedFaction = FACTION_NONE;
            PPCIV targetsOld;
            targetsOld.reserve(DEFAULT_DECK_RESERVE_SIZE);
            PPCARDINDEX tmp;
            EFFECT_ARGUMENT effect;
            UCHAR FusionMultiplier = 1;
            if (IsFusioned) {
                FusionMultiplier = 2;
            }

            UCHAR SrcPos = StructureIndex;
            if (Position > 0)
                SrcPos = (UCHAR)Position;

            bool bIsSelfMimic = false; // Chaosed Mimic indicator
            if (IsMimiced && Mimicer && (!Units.empty()))
            {
                for (LCARDS::iterator iter = Deck.begin(); iter != Deck.end(); iter++)
                    if (Mimicer == &(*iter))
                    {
                        bIsSelfMimic = true;
                        break;
                    }
            }

            //ActiveDeck *procDeck = ((!IsMimiced) || bIsSelfMimic) ? this : &Dest;
            ActiveDeck *procDeck = this;
            // The card acting currently, that is not necessary the card having the skills (e.g, mimic)
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

            // abilities that target units "active next turn" are smart enough to know that On Death and On Attacked should include Wait 1
            unsigned int activeNextTurnWait = (EffectType == EVENT_DIED || EffectType == EVENT_ATTACKED) ? 1 : 0;

            // TODO probably unnecessary to do all of this quest checking here;
            // should roll it up a level at some point.
            // Also, this could probably be another switch.

            UCHAR questAbilityCount = 0;
            AbilityEnum questAbilityId = SPECIAL_ATTACK; // TODO need better placeholder
            EFFECT_ARGUMENT questAbilityEffect = 0;
            TargetsCount questAbilityTargets = TARGETSCOUNT_ONE;

            if(QuestEffectId == BattleGroundEffect::friendlyFire && EffectType == EVENT_EMPTY) {
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
                    case TYPE_ACTION:
                    case TYPE_STRUCTURE:
                        // no action
                        break;
                    case TYPE_NONE:
                        throw LogicError("Illegal card type");
                }
            }

            if(QuestEffectId == BattleGroundEffect::genesis && EffectType == EVENT_EMPTY) {
                if(Src.GetType() == TYPE_COMMANDER) {
                    questAbilityId = ACTIVATION_SUMMON;
                    // get random assault card; make sure they are not special dev only cards
                    do {
                        questAbilityEffect = UCHAR(rand() % CARD_MAX_ID);
                    } while(questAbilityEffect == 0
                        || ((Card const * const)&pCDB[questAbilityEffect])->GetType() != TYPE_ASSAULT
                        || ((Card const * const)&pCDB[questAbilityEffect])->GetSet() == 0);
                    questAbilityTargets = TARGETSCOUNT_ONE;
                    questAbilityCount++;
                }
            }

            if(QuestEffectId == BattleGroundEffect::timeSurge && EffectType == EVENT_EMPTY) {
                if(Src.GetType() == TYPE_COMMANDER) {
                    questAbilityId = ACTIVATION_RUSH;
                    questAbilityEffect = 1;
                    questAbilityTargets = TARGETSCOUNT_ONE;
                    questAbilityCount++;
                }
            }

            if (    (    QuestEffectId == BattleGroundEffect::cloneProject
                      || QuestEffectId == BattleGroundEffect::splitFive
                    )
                 && (!IsMimiced)
                 && (Src.GetQuestSplit())
                 && (EffectType == EVENT_EMPTY)
               )
            {
                Src.SetQuestSplit(false); // remove mark
                questAbilityId = ACTIVATION_SPLIT;
                questAbilityEffect = 1;
                questAbilityTargets = TARGETSCOUNT_ONE;
                questAbilityCount++;
            }

            Ability::ConstPtr questAbility;
            if (questAbilityCount > 0) {
                questAbility = Ability::createAbility(questAbilityId
                                                     ,questAbilityTargets
                                                     ,FACTION_NONE
                                                     ,questAbilityEffect
                                                     ,EVENT_EMPTY
                                                     );
                assertLE(questAbilityCount,(UCHAR)1u);
            }

            for (UCHAR aindex=0;aindex<(ac+questAbilityCount);aindex++)
            {
                if (!IsMimiced)
                {
                    if (Src.GetEffect(ACTIVATION_JAM) > 0)
                        break; // card was jammed by payback (or chaos?)
                    if (Src.GetEffect(ACTIVATION_FREEZE) > 0 && !(EffectType == EVENT_DIED || EffectType == EVENT_ATTACKED))
                        break; // chaos-mimic-freeze makes this possible
                }

                // Need to check this every time card uses skill because it could be paybacked chaos
                bool chaos = procCard->GetEffect(ACTIVATION_CHAOS) != 0;
                ActiveDeck *targetDeck = chaos ? this : &EnemyDeck;

                if(aindex < ac) {
                    aid = Src.GetAbilityInOrder(aindex);

                    // ignore the commander's normal chaos
                    // TODO should probably move to the deck building code at some point...
                    if(aid == ACTIVATION_CHAOS && QuestEffectId == BattleGroundEffect::friendlyFire && Src.GetType() == TYPE_COMMANDER) {
                        continue;
                    }

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
                    infusedFaction = Src.GetEffect(ACTIVATION_INFUSE) ? idToFaction(Src.GetEffect(ACTIVATION_INFUSE)) : faction;
                    targetCount = Src.GetTargetCount(aid);
                } else {
                    aid = questAbilityId;
                    effect = questAbilityEffect;
                    targetCount = questAbilityTargets;
                    infusedFaction = faction = FACTION_NONE;
                }

                switch(aid) {
                case ACTIVATION_CLEANSE:
                    {
                        if (QuestEffectId == BattleGroundEffect::decay) { // decay disables all cleansing
                            break;
                        }

                        // P: I still have no idea what that fusion thing is...
                        effect *= FusionMultiplier;
                        
                        assertGT(effect,0u);

                        TargetSet targets;
                        getTargets(targets, this->Units);
                        filterTargetsByFaction(targets, infusedFaction);
                        removeTargetsThatCannotBeCleansed(targets);

                        bool isTributable = isInTargets(targets, *procCard);

                        pickRandomTargetsWithoutIntercept(targets,targetCount);

                        if (targets.empty()) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        BOOST_FOREACH(Target const & target, targets) {
                            PlayedCard & targetCard = target.card;
                            LOG(this->logger,abilitySupport(EffectType,Src,aid,targetCard,effect));

                            targetCard.Cleanse();
                            procCard->fsSpecial += effect;

                            if(isTributable && Tribute(&targetCard, procCard, procDeck, EffectType, aid, effect))
                            {
                                procCard->Cleanse();
                                targetCard.fsSpecial += effect;
                            }

                            CardPosition pos = target.position;
                            PlayedCard *oppositeCard = NULL;
                            if(pos < EnemyDeck.Units.size()) {
                                oppositeCard = &EnemyDeck.getUnitAt(pos);
                            }
                            if(oppositeCard != NULL && oppositeCard->CanEmulate(aid)) {
                                oppositeCard->Cleanse();
                                oppositeCard->fsSpecial += effect;
                                EnemyDeck.SkillProcs[DEFENSIVE_EMULATE]++;
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
                            GetTargets(Units,faction,targetsOld);
                            lc.DeckID = LogDeckID;
                        }
                        else
                        {
                            GetTargets(EnemyDeck.Units,faction,targetsOld);
                            lc.DeckID = EnemyDeck.LogDeckID;
                        }

                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,!chaos);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++)
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                EnemyDeck.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, false));
                                lc.CardID = vi->second;
                                vi->first->SetEffect(aid,vi->first->GetEffect(aid) + effect);

                                procCard->fsSpecial += effect;
                                LogAdd(LOG_CARD(procDeck->LogDeckID,procCard->GetType(),SrcPos),lc,aid,effect);

                                if (Payback(*targetDeck, vi->first, *procCard, EffectType, aid, effect, chaos))  // payback
                                {
                                    procCard->SetEffect(aid,procCard->GetEffect(aid) + effect);
                                    vi->first->fsSpecial += effect;
                                }
                            }
                    } break;

                case ACTIVATION_HEAL:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);
                        GetTargets(Units,infusedFaction,targetsOld);

                        PPCIV::iterator vi = targetsOld.begin();
                        while (vi != targetsOld.end())
                        {
                            if ((vi->first->GetHealth() == vi->first->GetMaxHealth()) || (vi->first->IsDiseased()))
                                vi = targetsOld.erase(vi);
                            else vi++;
                        }

                        // if something tributes this, are we a valid target?
                        bool bTributable = IsInTargets(procCard,&targetsOld);

                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,false);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (vi = targetsOld.begin();vi != targetsOld.end();vi++)
                        {
                            LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));

                            procCard->fsHealed += vi->first->Heal(effect,QuestEffectId);
                            //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                            if(bTributable && Tribute(vi->first, procCard, procDeck, EffectType, aid, effect))
                            {
                                vi->first->fsHealed += procCard->Heal(effect,QuestEffectId);
                            }

                            UCHAR pos = vi->second;
                            PlayedCard *oppositeCard = NULL;
                            if(pos < EnemyDeck.Units.size()) {
                                oppositeCard = &EnemyDeck.getUnitAt(pos);
                            }
                            if(oppositeCard != NULL && oppositeCard->CanEmulate(aid)) {
                                oppositeCard->fsHealed += oppositeCard->Heal(effect,QuestEffectId);
                                EnemyDeck.SkillProcs[DEFENSIVE_EMULATE]++;
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
                            targetsOld.clear();
                            // If we are not left most, add unit left of us a target
                            if (Position > 0) {
                                targetsOld.push_back(PPCARDINDEX(&(this->getUnitAt(Position-1)),Position-1));
                            }
                            // we are a target
                            targetsOld.push_back(PPCARDINDEX(&(this->getUnitAt(Position)),Position));
                            // if there is a unit right of us, add it as a target
                            if ((DWORD)Position+1 < Units.size()) {
                                targetsOld.push_back(PPCARDINDEX(&(this->getUnitAt(Position+1)),Position+1));
                            }

                            // there should always be a target for supply: self
                            assertGT(targetsOld.size(),0u);

                            PPCIV::iterator vi = targetsOld.begin();
                            if (targetsOld.size() > 0)	{
                                // check each target for disease or full health
                                while (vi != targetsOld.end())	{
                                    if ((vi->first->GetHealth() == vi->first->GetMaxHealth())) {
                                        vi = targetsOld.erase(vi);
                                    } else if (!vi->first->IsAlive()) {
                                        vi = targetsOld.erase(vi);
                                    } else if (vi->first->IsDiseased()) {
                                        // remove diseased targets
                                        LOG(this->logger,abilityFailDisease(EffectType,aid,Src,*(vi->first),IsMimiced,FACTION_NONE,effect));
                                        vi = targetsOld.erase(vi);
                                    } else {
                                        vi++;
                                    }
                                }
                            }

                            // do we still have targets?
                            if (!targetsOld.empty()) {
                                procDeck->SkillProcs[aid]++;
                            } else {
                                // no targets
                                LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,false,FACTION_NONE,effect));
                                break;
                            }

                            //FIXME: That variable is unused, yet has a large right hand side...
                            bool bTributable = IsInTargets(procCard,&targetsOld);

                            // now comes the actual healing
                            for (vi = targetsOld.begin(); vi != targetsOld.end(); vi++) {
                                PlayedCard & target = *(vi->first);
                                assertX(target.IsDefined());
                                LOG(this->logger,abilitySupport(EffectType,Src,aid,target,effect));

                                procCard->fsHealed += vi->first->Heal(effect,QuestEffectId);
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                if (bTributable && Tribute(vi->first, procCard, procDeck, EffectType, aid,effect)) {
                                    vi->first->fsHealed += procCard->Heal(effect,QuestEffectId);
                                }

                                UCHAR pos = vi->second;
                                PlayedCard *oppositeCard = NULL;
                                if(pos < EnemyDeck.Units.size()) {
                                    oppositeCard = &EnemyDeck.getUnitAt(pos);
                                }
                                if(oppositeCard != NULL && oppositeCard->CanEmulate(aid)) {
                                    oppositeCard->fsHealed += oppositeCard->Heal(effect, QuestEffectId);
                                    EnemyDeck.SkillProcs[DEFENSIVE_EMULATE]++;
                                }
                            }
                        }
                    } break;

                case ACTIVATION_PROTECT:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);

                        GetTargets(Units,infusedFaction,targetsOld);

                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,false);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        bool bTributable = IsInTargets(procCard,&targetsOld);

                        procDeck->SkillProcs[aid]++;

                        PPCIV::iterator vi = targetsOld.begin();
                        for (vi = targetsOld.begin();vi != targetsOld.end();vi++)
                        {
                            LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                            vi->first->Protect(effect);
                            procCard->fsSpecial += effect;

                            //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                            if(bTributable && Tribute(vi->first, procCard, procDeck, EffectType, aid, effect))
                            {
                                procCard->Protect(effect);
                                vi->first->fsSpecial += effect;
                            }

                            UCHAR pos = vi->second;
                            PlayedCard *oppositeCard = NULL;
                            if(pos < EnemyDeck.Units.size()) {
                                oppositeCard = &EnemyDeck.getUnitAt(pos);
                            }
                            if(oppositeCard != NULL && oppositeCard->CanEmulate(aid)) {
                                oppositeCard->Protect(effect);
                                oppositeCard->fsSpecial += effect;
                                EnemyDeck.SkillProcs[DEFENSIVE_EMULATE]++;
                            }
                        }
                    } break;

                case ACTIVATION_JAM:
                    {
                        // infuse is processed on the upper level
                        assertGT(effect,0u);

                        if (chaos)
                            GetTargets(Units,faction,targetsOld);
                        else
                            GetTargets(EnemyDeck.Units,faction,targetsOld);

                        // HACK: Eventually need to check for intercept *after* the PROC50 below
                        int interceptCount = EnemyDeck.SkillProcs[DEFENSIVE_INTERCEPT];

                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_JAM, 0};
                        FilterTargets(targetsOld,skipEffects,NULL,-1,1,-1,!chaos);
                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,!chaos);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,(chaos),faction,effect));
                            break;
                        }

                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++)
                        {
                            if (PROC50)
                            {
                                // Intercept

                                if (Evade(vi->first, QuestEffectId, chaos))
                                {
                                    LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                    EnemyDeck.SkillProcs[DEFENSIVE_EVADE]++;
                                }
                                else
                                {
                                    LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                    vi->first->SetEffect(aid,effect);

                                    procCard->fsSpecial += effect;
                                    procDeck->SkillProcs[aid]++;

                                    if (procCard->GetEffect(ACTIVATION_JAM) == 0
                                        && Payback(*targetDeck, vi->first, *procCard, EffectType, aid, effect, chaos))  // payback is it 1/2 or 1/4 chance to return jam with payback?
                                    {
                                        procCard->SetEffect(aid,effect);
                                        vi->first->fsSpecial += effect;
                                    }
                                }
                            }
                            else
                            {
                                EnemyDeck.SkillProcs[DEFENSIVE_INTERCEPT] = interceptCount; // undo any intercept proc that may have happened
                            }
                        }
                    } break;

                case ACTIVATION_FREEZE:
                    {
                        assertGT(effect,0u);

                        if (chaos)
                            GetTargets(Units,faction,targetsOld);
                        else
                            GetTargets(EnemyDeck.Units,faction,targetsOld);

                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_FREEZE, 0};
                        FilterTargets(targetsOld,skipEffects,NULL,-1,-1,-1,false);
                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,!chaos);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++)
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                EnemyDeck.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                vi->first->SetEffect(aid,effect);
                                procCard->fsSpecial += effect;
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                if (Payback(*targetDeck, vi->first, *procCard, EffectType, aid, effect, chaos))  // payback
                                {
                                    procCard->SetEffect(aid,effect);
                                    vi->first->fsSpecial += effect;
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

                        if (chaos || (QuestEffectId == BattleGroundEffect::copyCat))
                            GetTargets(Units,faction,targetsOld);
                        else
                            GetTargets(EnemyDeck.Units,faction,targetsOld);

                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,!chaos && (QuestEffectId != BattleGroundEffect::copyCat));

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++)
                            if (QuestEffectId != BattleGroundEffect::copyCat && Evade(vi->first, QuestEffectId, chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                EnemyDeck.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                if (chaos)
                                    ApplyEffects(QuestEffectId,EVENT_EMPTY,*vi->first,Position,*this,true,false,&Src);
                                else
                                    ApplyEffects(QuestEffectId,EVENT_EMPTY,*vi->first,Position,EnemyDeck,true,false,&Src);
                            }
                    } break;

                case ACTIVATION_RALLY:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);

                        TargetSet friendlyTargets;
                        getTargets(friendlyTargets, this->Units);
                        filterTargetsByFaction(friendlyTargets, infusedFaction);
                        removeTargetsThatCannotAttack(friendlyTargets);
                        removeTargetsThatStartedAttacking(friendlyTargets);
                        removeTargetsWithDelayOutsideOf(friendlyTargets, -1, activeNextTurnWait);
                        
                        bool const isTributable = isInTargets(friendlyTargets, *procCard);

                        pickRandomTargetsWithoutIntercept(friendlyTargets,targetCount);

                        

                        if (friendlyTargets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }
                        
                        procDeck->SkillProcs[aid]++;

                        for (TargetSet::iterator iter = friendlyTargets.begin()
                            ;iter != friendlyTargets.end()
                            ;iter++
                            )
                        {
                            Target const & target = *iter;
                            PlayedCard & targetCard = target.card;
                            LOG(this->logger,abilitySupport(EffectType,Src,aid,targetCard,effect));                            
                            targetCard.Rally(effect);
                            procCard->fsSpecial += effect;
                            //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                            if(isTributable && Tribute(&targetCard, procCard, procDeck, EffectType, aid, effect))
                            {
                                procCard->Rally(effect);
                                targetCard.fsSpecial += effect;
                            }

                            CardPosition pos = target.position;
                            PlayedCard * oppositeCard = NULL;
                            if(pos < EnemyDeck.Units.size()) {
                                // FIXME that may be incorrect
                                oppositeCard = &EnemyDeck.getUnitAt(pos);
                            }
                            if(oppositeCard != NULL && oppositeCard->CanEmulate(aid)) {
                                oppositeCard->Rally(effect);
                                oppositeCard->fsSpecial++;
                                EnemyDeck.SkillProcs[DEFENSIVE_EMULATE]++;
                            }
                        }
                    } break;

                case ACTIVATION_RECHARGE:
                    {
                        if (Src.GetAbility(aid) > 0)
                            if (PROC50) {
                                LOG(this->logger,abilitySupport(EffectType,Src,aid,Src,1));
                                Src.SetEffect(ACTIVATION_RECHARGE, 1);
                                Deck.push_back(Src);
                                }
                    } break;

                case ACTIVATION_REPAIR:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);

                        GetTargets(Structures,infusedFaction,targetsOld);

                        PPCIV::iterator vi = targetsOld.begin();
                        while (vi != targetsOld.end())
                        {
                            if (vi->first->GetHealth() == vi->first->GetMaxHealth())
                                vi = targetsOld.erase(vi);
                            else vi++;
                        }

                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,false);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++)
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
                        LOG(this->logger,abilityOffensive(EffectType,Src,aid,EnemyDeck.Commander,effect));
                        Src.fsDmgDealt += EnemyDeck.Commander.SufferDmg(QuestEffectId,effect);
                        DamageToCommander += effect;
                        FullDamageToCommander += effect;
                    } break;

                case ACTIVATION_SIEGE:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);
                        if (chaos)
                            GetTargets(Structures,faction,targetsOld);
                        else
                            GetTargets(EnemyDeck.Structures,faction,targetsOld);

                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,false);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++)

                            if (Evade(vi->first,QuestEffectId,chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect,true));
                                vi->first->fsAvoided += effect;
                                EnemyDeck.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                UCHAR overkill = 0;
                                UCHAR sdmg = vi->first->SufferDmg(QuestEffectId,effect,0,0,0,&overkill);
                                if(chaos) {
                                    procDeck->CheckDeathEvents(*vi->first,EnemyDeck);
                                } else {
                                    EnemyDeck.CheckDeathEvents(*vi->first,*this);
                                }
                                procCard->fsDmgDealt += sdmg;
                                procCard->fsOverkill += overkill;
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
                            ApplyEffects(QuestEffectId,EVENT_PLAYED,Units.back(),-1,EnemyDeck);
                            LOG(this->logger,abilitySupport(EffectType,Src,ACTIVATION_SPLIT,Src,effect));
                        }
                    } break;

                case ACTIVATION_STRIKE:
                    {
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);
                        assertGT(effect,0u);

                        if (chaos) {
                            GetTargets(Units,faction,targetsOld);
                        } else {
                            GetTargets(EnemyDeck.Units,faction,targetsOld);
                        }

                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,!chaos);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        assertX(!targetsOld.empty()); // Targets should never be empty at this point

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++) {
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                // evaded
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                vi->first->fsAvoided += effect;
                                EnemyDeck.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                UCHAR overkill = 0;
                                UCHAR sdmg = vi->first->StrikeDmg(QuestEffectId,effect,&overkill);
                                EnemyDeck.CheckDeathEvents(*vi->first,*this);
                                procCard->fsDmgDealt += sdmg;
                                procCard->fsOverkill += overkill;
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                if (Payback(*targetDeck, vi->first, *procCard, EffectType, aid, effect, chaos))  // payback
                                {
                                    UCHAR overkill = 0;
                                    vi->first->fsDmgDealt += procCard->StrikeDmg(QuestEffectId,effect,&overkill);
                                    CheckDeathEvents(*procCard,*procDeck);
                                    vi->first->fsOverkill += overkill;
                                }
                            }
                        }
                    } break;

                case ACTIVATION_SUMMON:
                    {
                        assertGT(effect,0u);
                        assertLT(effect,(UINT)CARD_MAX_ID);
                        assertX(pCDB != NULL);

                        procDeck->SkillProcs[aid]++;

                        Card const * const summonedCard = &pCDB[effect];
                        if(summonedCard->GetType() == TYPE_ASSAULT) {
                            Units.push_back(summonedCard);
                            LOG(this->logger,abilitySummon(EffectType,Src,Units.back()));
                            Units.back().SetIsSummoned(true);
                            Units.back().SetCardSkillProcBuffer(SkillProcs);
                            // TODO this is where the fix for Decay on Summoning needs to happen
                            //ApplyEffects(QuestEffectId,EVENT_PLAYED,Units.back(),-1,Dest);
                        } else if (summonedCard->GetType() == TYPE_STRUCTURE) {
                            Structures.push_back(summonedCard);
                            LOG(this->logger,abilitySummon(EffectType,Src,Structures.back()));
                            Structures.back().SetIsSummoned(true);
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
                        PlayCard(summonedCard, EnemyDeck);
                    } break;

                case ACTIVATION_WEAKEN:
                    {
                        assertGT(effect,0u);
                        effect *= FusionMultiplier;
                        effect += procCard->GetEffect(ACTIVATION_AUGMENT);

                        if (chaos) {
                            GetTargets(Units,faction,targetsOld);
                        } else {
                            GetTargets(EnemyDeck.Units,faction,targetsOld);
                        }

                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_JAM, ACTIVATION_FREEZE, DMGDEPENDANT_IMMOBILIZE, DEFENSIVE_STUN, 0};
                        int const maxWait = EffectType == EVENT_ATTACKED ? 0 : 1;
                        FilterTargets(targetsOld,skipEffects,NULL,-1,maxWait,1,!chaos);
                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,!chaos);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++)
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                EnemyDeck.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                UCHAR we = vi->first->Weaken(effect);
                                procCard->fsSpecial += we;
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                if (procCard->GetAttack() > 0
                                    && Payback(*targetDeck, vi->first, *procCard, EffectType, aid, effect, chaos))  // payback
                                {
                                    vi->first->fsSpecial += procCard->Weaken(effect);
                                }
                            }
                    } break;

                case ACTIVATION_CHAOS:
                    {
                        Ability::ConstPtr ability;
                        if (aindex < ac) {
                            ability = Src.getAbility(aindex);
                        } else {
                            ability = questAbility;
                        }
                        ability->executeAbility(EffectType                        
                                               ,*procCard
                                               ,*procDeck
                                               ,Position
                                               ,EnemyDeck
                                               ,QuestEffectId
                                               ,chaos
                                               ,IsFusioned
                                               );
                        #if 0
                        assertGT(effect,0u);

                        TargetSet targets;
                        // is the acting card chaosed
                        if (chaos) {
                            GetTargets(this->Units, faction, targetsOld);
                        } else {
                            // P: does chaosed chaos respect faction modified? Code says yes.
                            GetTargets(EnemyDeck.Units, faction, targetsOld);
                        }

                        // targets with one of these are not valid
                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_JAM, ACTIVATION_FREEZE, ACTIVATION_CHAOS, 0};
                        FilterTargets(targetsOld, skipEffects, NULL, -1, 1, -1, !chaos);
                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,!chaos);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        // for each target
                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++)
                            if (Evade(vi->first, QuestEffectId, chaos))
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect, true));
                                EnemyDeck.SkillProcs[DEFENSIVE_EVADE]++;
                            }
                            else
                            {
                                LOG(this->logger,abilityOffensive(EffectType,Src,aid,*(vi->first),effect));
                                vi->first->SetEffect(ACTIVATION_CHAOS,effect);
                                procCard->fsSpecial += effect;
                                //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                                if (procCard->GetEffect(ACTIVATION_CHAOS) == 0
                                    && Payback(*targetDeck, vi->first, *procCard, EffectType, aid, effect, chaos))  // payback
                                {
                                    procCard->SetEffect(ACTIVATION_CHAOS, effect);
                                    vi->first->fsSpecial += effect;
                                }
                            }
                        #endif
                    } break;
                case ACTIVATION_RUSH:
                    {
                        Ability::ConstPtr ability;
                        if (aindex < ac) {
                            ability = Src.getAbility(aindex);
                        } else {
                            ability = questAbility;
                        }
                        ability->executeAbility(EffectType
                                               ,*procCard
                                               ,*procDeck
                                               ,Position
                                               ,EnemyDeck
                                               ,QuestEffectId
                                               ,chaos
                                               ,IsFusioned
                                               );
                        #if 0
                        assertGT(effect,0u);
                        TargetSet targets;
                        GetTargets(Units,infusedFaction,targetsOld);
                        #error reimplement me
                        FilterTargets(targetsOld,NULL,NULL,1,-1,-1,false);

                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,false);

                        if (targets.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++)
                        {
                            LOG(this->logger,abilityOffensive(EffectType,Src,ACTIVATION_RUSH,*(vi->first),effect));
                            vi->first->Rush(effect);
                            Src.fsSpecial += effect;

                            UCHAR pos = vi->second;
                            PlayedCard *oppositeCard = NULL;
                            if(pos < EnemyDeck.Units.size()) {
                                oppositeCard = &EnemyDeck.getUnitAt(pos);
                            }
                            if(oppositeCard != NULL && oppositeCard->CanEmulate(aid)) {
                                oppositeCard->Rush(effect);
                                oppositeCard->fsSpecial += effect;
                                EnemyDeck.SkillProcs[DEFENSIVE_EMULATE]++;
                            }
                        }
                        #endif
                    } break;
                case SPECIAL_BACKFIRE:
                    {
                        LOG(this->logger,cardDamaged(procDeck->Commander,SPECIAL_BACKFIRE,procCard->GetAbility(SPECIAL_BACKFIRE)));
                        procDeck->Commander.SufferDmg(QuestEffectId,procCard->GetAbility(SPECIAL_BACKFIRE));
                        EnemyDeck.DamageToCommander += procCard->GetAbility(SPECIAL_BACKFIRE);
                        EnemyDeck.FullDamageToCommander += procCard->GetAbility(SPECIAL_BACKFIRE);
                        procDeck->SkillProcs[SPECIAL_BACKFIRE]++;
                        LogAdd(LOG_CARD(procDeck->LogDeckID,TYPE_ASSAULT,Position),LOG_CARD(procDeck->LogDeckID,TYPE_COMMANDER,0),SPECIAL_BACKFIRE,procCard->GetAbility(SPECIAL_BACKFIRE));
                    } break;
                case SPECIAL_BLITZ:
                    {
                        // TODO can Blitz be Jammed or Freezed?
                        UCHAR targetPos = Position;

                        if(targetPos < EnemyDeck.Units.size()) {
                            PlayedCard oppositeCard = EnemyDeck.getUnitAt(targetPos);
                            if ((oppositeCard.IsAlive())
                                && (oppositeCard.GetWait() == 0)
                                && ((oppositeCard.GetFaction() == faction) || (faction == FACTION_NONE))
                                )
                            {
                                Src.SetEffect(aid,effect);
                                procDeck->SkillProcs[SPECIAL_BLITZ]++;
                                LOG(this->logger,abilitySupport(EffectType,Src,aid,Src,effect));
                            } else {
                                // TODO probably want a more appropriate fail message
                                LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            }
                        } else {
                            // TODO probably want a more appropriate fail message
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                        }

                    } break;
                case ACTIVATION_AUGMENT:
                    {
                        effect *= FusionMultiplier;
                        assertGT(effect,0u);

                        GetTargets(Units,infusedFaction,targetsOld);

                        EFFECT_ARGUMENT skipEffects[] = {ACTIVATION_JAM, ACTIVATION_FREEZE, 0};
                        EFFECT_ARGUMENT targetSkills[] = {ACTIVATION_ENFEEBLE, ACTIVATION_HEAL, ACTIVATION_PROTECT, ACTIVATION_RALLY, ACTIVATION_REPAIR, ACTIVATION_SIEGE, ACTIVATION_STRIKE, ACTIVATION_SUPPLY, ACTIVATION_WEAKEN, 0};

                        // friendly fire "gives" all units Strike 1
                        if(QuestEffectId == BattleGroundEffect::friendlyFire) {
                            FilterTargets(targetsOld,skipEffects,NULL,-1,activeNextTurnWait,-1,true);
                        } else {
                            FilterTargets(targetsOld,skipEffects,targetSkills,-1,activeNextTurnWait,-1,true);
                        }

                        bool bTributable = IsInTargets(procCard,&targetsOld);

                        RandomizeTarget(targetsOld,targetCount,EnemyDeck,false);

                        if (targetsOld.size() <= 0) {
                            LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,chaos,faction,effect));
                            break;
                        }

                        procDeck->SkillProcs[aid]++;

                        for (PPCIV::iterator vi = targetsOld.begin();vi != targetsOld.end();vi++)
                        {
                            LOG(this->logger,abilitySupport(EffectType,Src,aid,*(vi->first),effect));
                            vi->first->Augment(effect);
                            procCard->fsSpecial += effect;
                            //LogAdd(LOG_CARD(LogDeckID,procCard->GetType(),SrcPos),lc,aid);

                            if(bTributable && Tribute(vi->first, procCard, procDeck, EffectType, aid, effect))
                            {
                                procCard->Augment(effect);
                                vi->first->fsSpecial += effect;
                            }

                            UCHAR pos = vi->second;
                            PlayedCard *oppositeCard = NULL;
                            if(pos < EnemyDeck.Units.size()) {
                                oppositeCard = &EnemyDeck.getUnitAt(pos);
                            }
                            if(oppositeCard != NULL && oppositeCard->CanEmulate(aid)) {
                                oppositeCard->Augment(effect);
                                oppositeCard->fsSpecial += effect;
                                EnemyDeck.SkillProcs[DEFENSIVE_EMULATE]++;
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
                        applyDamageDependentEffectOnAttack(QuestEffectId, Src, aid, effectArgument, EnemyDeck, *target);
                    }
                } // end switch

                targetsOld.clear();

            } // end for(aindex:ac)


        }



        

        
    }
}
