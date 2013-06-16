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
                     && ActiveDeck::Payback(oppositeDeck, &targetCard, actingCard, condition, this->ability, this->argument, isChaosed)
                   )  // payback
                {
                    // TODO does paybacked weaken affect targets with attack <= 0?
                    actingCard.addEffect(this->ability, this->argument);
                    targetCard.fsSpecial += this->argument;
                }
            }
        }

    }
}
