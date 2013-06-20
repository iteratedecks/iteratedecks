#include "abilityWeaken.hpp"
#include "playedCard.hpp"
#include "activeDeck.hpp"
#include <boost/foreach.hpp>
#include "Logger.hpp"

namespace IterateDecks {
    namespace Core {

        AbilityWeaken::AbilityWeaken(AbilityEnum abilityType
                                    ,TargetCount targetCount
                                    ,Faction targetFaction
                                    ,AbilityArgument argument
                                    ,EventCondition conditions
                                    )
        : Ability(abilityType, targetCount, targetFaction, argument, conditions)
        {
        }

        void
        AbilityWeaken::executeAbilityForOneTarget
                        (EventCondition condition
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
                targetCard.addEffect(this->ability,this->argument);
                actingCard.fsSpecial += this->argument;

                if (    !isChaosed
                     && actingCard.GetAttack() > 0
                     && ActiveDeck::Payback(oppositeDeck, &targetCard, actingCard, condition, this->ability, this->argument, isChaosed)
                   )  // payback
                {
                    // TODO does paybacked weaken affect targets with attack <= 0?
                    actingCard.addEffect(this->ability, this->argument);
                    targetCard.fsSpecial += this->argument;
                }
            }
        }

        TargetSet
        AbilityWeaken::findTargets(EventCondition eventCondition
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
            removeTargetsThatCannotAttack(targets);
            removeTargetsWithZeroOrLessAttack(targets);

            // TODO: Needs testing
            throw LogicError("Not implemented.");
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
