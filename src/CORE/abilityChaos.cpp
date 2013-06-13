#include "abilityChaos.hpp"
#include <boost/foreach.hpp>
#include "assert.hpp"
#include "playedCard.hpp"
#include "Logger.hpp"
#include "activeDeck.hpp"

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
        AbilityChaos::executeAbility(EventCondition condition
                               ,PlayedCard & actingCard
                               ,ActiveDeck & actingDeck
                               ,CardPosition position
                               ,ActiveDeck & oppositeDeck
                               ,BattleGroundEffect battleGroundEffect
                               ,bool isChaosed
                               ,bool isFusioned
                               ) const
        {
            TargetSet targets = this->findTargets(actingDeck, oppositeDeck, isChaosed);
            BOOST_FOREACH(Target target, targets) {
                PlayedCard & targetCard = target.card;
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

        }

    }
}
