#include "ability.hpp"
#include "exceptions.hpp"
#include <sstream>

#include "abilityChaos.hpp"
#include "abilityWeaken.hpp"

#include "activeDeck.hpp"
#include "Logger.hpp"
#include <boost/foreach.hpp>

namespace IterateDecks {
    namespace Core {

        class NotImplementedAbility : public Ability {
            public:
                NotImplementedAbility(AbilityEnum abilityType
                                     ,TargetCount targetCount
                                     ,Faction targetFaction
                                     ,AbilityArgument argument
                                     ,EventCondition condition
                                     )
                : Ability(abilityType, targetCount, targetFaction, argument, condition)
                {}

                void
                executeAbilityCheckCondition(PlayedCard & actingCard
                              ,ActiveDeck & actingDeck
                              ,CardPosition position
                              ,ActiveDeck & oppositeDeck
                              ,BattleGroundEffect battleGroundEffect
                              ,bool isChaosed
                              ,bool isFusioned
                              ) const
                {
                    std::stringstream ssMessage;
                    ssMessage << "Not implemented ability: ";
                    ssMessage << this->ability;
                    throw LogicError(ssMessage.str());
                }

                void
                executeAbility(EventCondition condition
                              ,PlayedCard & actingCard
                              ,ActiveDeck & actingDeck
                              ,CardPosition position
                              ,ActiveDeck & oppositeDeck
                              ,BattleGroundEffect battleGroundEffect
                              ,bool isChaosed
                              ,bool isFusioned
                              ) const
                {
                    std::stringstream ssMessage;
                    ssMessage << "Not implemented ability: ";
                    ssMessage << this->ability;
                    throw LogicError(ssMessage.str());
                }

                void executeAbilityForOneTarget(EventCondition condition
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
                    std::stringstream ssMessage;
                    ssMessage << "Not implemented ability: ";
                    ssMessage << this->ability;
                    throw LogicError(ssMessage.str());
                }

                TargetSet
                findTargets(EventCondition eventCondition
                           ,ActiveDeck & actingDeck
                           ,CardPosition position
                           ,ActiveDeck & oppositeDeck
                           ,bool isChaosed
                           ) const
                {
                    std::stringstream ssMessage;
                    ssMessage << "Not implemented ability: ";
                    ssMessage << this->ability;
                    throw LogicError(ssMessage.str());
                }
        };

        Ability::Ability(AbilityEnum abilityType
                        ,TargetCount targetCount
                        ,Faction targetFaction
                        ,AbilityArgument argument
                        ,EventCondition condition
                        )
        : ability(abilityType)
        , targetCount(targetCount)
        , targetFaction(targetFaction)
        , argument(argument)
        , eventConditions(condition)
        {}

        Ability::~Ability() {}

        void
        Ability::executeAbilityCheckConditions(EventCondition eventCondition
                                              ,PlayedCard & actingCard
                                              ,ActiveDeck & actingDeck
                                              ,CardPosition position
                                              ,ActiveDeck & oppositeDeck
                                              ,BattleGroundEffect battleGroundEffect
                                              ,bool isChaosed
                                              ,bool isFusioned
                                              ) const
        {
            throw LogicError("Not implemented.");
        }

        void
        Ability::executeAbility(EventCondition condition
                               ,PlayedCard & actingCard
                               ,ActiveDeck & actingDeck
                               ,CardPosition position
                               ,ActiveDeck & oppositeDeck
                               ,BattleGroundEffect battleGroundEffect
                               ,bool isChaosed
                               ,bool isFusioned
                               ) const
        {
            TargetSet targets = this->findTargets(condition, actingDeck, position, oppositeDeck, isChaosed);
            if (targets.size() <= 0) {
                LOG(actingDeck.logger,abilityFailNoTarget(condition,this->ability,actingCard,isChaosed,this->targetFaction,this->argument));
                return;
            } else {
                actingDeck.SkillProcs[this->ability]++;
                BOOST_FOREACH(Target target, targets) {
                    this->executeAbilityForOneTarget(condition, actingCard, actingDeck, position
                                                    ,target.card, oppositeDeck, target.position
                                                    ,battleGroundEffect, isChaosed, isFusioned
                                                    );
                }
            }
        }

        Ability::Ptr
        Ability::createAbility(AbilityEnum abilityType
                              ,TargetCount targetCount
                              ,Faction targetFaction
                              ,AbilityArgument argument
                              ,EventCondition condition
                              )
        {
            switch(abilityType) {
                case ACTIVATION_CHAOS:
                    return Ability::Ptr(
                        new AbilityChaos(abilityType,targetCount,targetFaction,argument,condition)
                    );
                case ACTIVATION_WEAKEN:
                    return Ability::Ptr(
                        new AbilityWeaken(abilityType,targetCount,targetFaction,argument,condition)
                    );
                default: {
                        return Ability::Ptr(
                            new NotImplementedAbility(abilityType
                                                     ,targetCount
                                                     ,targetFaction
                                                     ,argument
                                                     ,condition)
                        );
                    }
            }
        }

    }
}
