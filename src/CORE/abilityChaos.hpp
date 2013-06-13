#ifndef ITERATEDECKS_CORE_ABILITYCHAOS_HPP
    #define ITERATEDECKS_CORE_ABILITYCHAOS_HPP

    #include "ability.hpp"

    namespace IterateDecks {
        namespace Core {

            class AbilityChaos : public Ability {
                public:
                    AbilityChaos(AbilityEnum abilityType
                                ,TargetCount targetCount
                                ,Faction targetFaction
                                ,AbilityArgument argument
                                ,EventCondition condition
                                );

                    void
                    executeAbility(EventCondition condition
                                  ,PlayedCard & actingCard
                                  ,ActiveDeck & actingDeck
                                  ,CardPosition position
                                  ,ActiveDeck & oppositeDeck
                                  ,BattleGroundEffect battleGroundEffect
                                  ,bool isChaosed
                                  ,bool isFusioned
                                  ) const;

                    TargetSet
                    findTargets
                        (EventCondition eventCondition
                        ,ActiveDeck & actingDeck
                        ,CardPosition position
                        ,ActiveDeck & oppositeDeck
                        ,bool isChaosed
                        ) const;

            };
        }
    }
#endif

