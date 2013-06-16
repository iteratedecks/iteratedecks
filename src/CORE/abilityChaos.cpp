#include "abilityChaos.hpp"
#include <boost/foreach.hpp>
#include "assert.hpp"
#include "playedCard.hpp"
#include "Logger.hpp"
#include "activeDeck.hpp"
#include "targetManagement.hpp"

namespace IterateDecks {
    namespace Core {

        AbilityChaos::AbilityChaos(AbilityEnum abilityType
                                  ,TargetCount targetCount
                                  ,Faction targetFaction
                                  ,AbilityArgument argument
                                  ,EventCondition conditions
                                  )
        : Ability(abilityType, targetCount, targetFaction, argument, conditions)
        {
            assertEQ(argument,1u);
        }


        void
        AbilityChaos::executeAbilityForOneTarget(EventCondition condition
                        ,PlayedCard & actingCard
                        ,ActiveDeck & actingDeck
                        ,CardPosition actingPosition
                        ,PlayedCard & targetCard
                        ,ActiveDeck & oppositeDeck
                        ,CardPosition targetPosition
                        ,BattleGroundEffect battleGroundEffect
                        ,bool isChaosed
                        ,bool isFusioned
                               ) const
        {
            if (ActiveDeck::Evade(&targetCard, battleGroundEffect, isChaosed)) {
                LOG(actingDeck.logger,abilityOffensive(condition,actingCard,this->ability,targetCard,this->argument, true));
                oppositeDeck.SkillProcs[DEFENSIVE_EVADE]++;
            } else {
                LOG(actingDeck.logger,abilityOffensive(condition,actingCard,this->ability,targetCard,this->argument));
                targetCard.SetEffect(this->ability,this->argument);
                actingCard.fsSpecial += this->argument;

                if (    !isChaosed
                     && ActiveDeck::Payback(oppositeDeck, &targetCard, actingCard, condition, this->ability, this->argument, isChaosed)
                   )  // payback
                {
                    actingCard.SetEffect(this->ability, this->argument);
                    targetCard.fsSpecial += this->argument;
                }
            }
        }

        TargetSet
        AbilityChaos::findTargets(EventCondition eventCondition
                                 ,ActiveDeck & actingDeck
                                 ,CardPosition position
                                 ,ActiveDeck & oppositeDeck
                                 ,bool isChaosed
                                 ) const
        {
            TargetSet targets;
            if (isChaosed) {
                IterateDecks::Core::getTargets(targets, actingDeck.Units);
            } else {
                IterateDecks::Core::getTargets(targets, oppositeDeck.Units);
            }
            filterTargetsByFaction(targets, this->targetFaction);
            removeTargetsThatCannotAct(targets);

            // Don't apply this to units which are already affected.
            removeTargetsWithEffect(targets, this->ability);

            if (eventCondition == EVENT_EMPTY || eventCondition == EVENT_PLAYED) {
                removeTargetsWithDelayOutsideOf(targets,0,1);
            } else if (eventCondition == EVENT_DIED) {
                removeTargetsWithDelayOutsideOf(targets,0,0);
            } else if (eventCondition == EVENT_ATTACKED ) {
                removeTargetsWithDelayOutsideOf(targets,0,0);
                // only targets units that have not yet attacked
                // P: this somehow applies to opposite side aswell, expect this to be a bug in tyrant.
                //    The reason is not relevant, though.
                removeTargetsLeftOfOrAtSamePosition(targets, position);
            } else if (eventCondition == EVENT_KILL) {
                // Moraku's guide claims on kill can only target with delay 0, not with 1.
                // P: That does not make any sense, so lets look into this.
                // P: Yep, thats wrong again.
                // P: Basically on kill tagets the same as normal.
                removeTargetsWithDelayOutsideOf(targets,0,1);
            } else {
                throw LogicError("Switch case failed.");
            }
            return targets;
        }

    }
}
