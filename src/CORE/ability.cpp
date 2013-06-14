#include "ability.hpp"
#include "exceptions.hpp"
#include <sstream>

#include "abilityChaos.hpp"
#include "abilityWeaken.hpp"

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
                executeAbility(PlayedCard & actingCard
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
