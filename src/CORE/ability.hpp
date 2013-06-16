#ifndef ITERATEDECKS_CORE_ABILITY_HPP
    #define ITERATEDECKS_CORE_ABILITY_HPP

    #include <memory>
    #include "simpleTypes.hpp"
    #include "constants.hpp"
    #include "playedCard.forward.hpp"
    #include "activeDeck.forward.hpp"
    #include "targetManagement.hpp"

    namespace IterateDecks {
        namespace Core {

            class Ability {
                public:
                    AbilityEnum ability;
                    TargetCount targetCount;
                    Faction targetFaction;
                    AbilityArgument argument;
                    EventCondition eventConditions;

                public:
                    typedef std::shared_ptr<Ability> Ptr;
                    typedef std::shared_ptr<Ability const> ConstPtr;

                protected:
                    Ability(AbilityEnum abilityType, TargetCount targetCount, Faction targetFaction, AbilityArgument argument, EventCondition condition);
                public:
                    static Ability::Ptr createAbility(AbilityEnum abilityType
                                                     ,TargetCount targetCount
                                                     ,Faction targetFaction
                                                     ,AbilityArgument argument
                                                     ,EventCondition condition
                                                     );
                    ~Ability();

                public:
                    void executeAbilityCheckConditions
                        (EventCondition eventCondition
                        ,PlayedCard & actingCard
                        ,ActiveDeck & actingDeck
                        ,CardPosition position
                        ,ActiveDeck & oppositeDeck
                        ,BattleGroundEffect battleGroundEffect
                        ,bool isChaosed
                        ,bool isFusioned
                        ) const;

                    virtual
                    void executeAbility
                        (EventCondition condition
                        ,PlayedCard & actingCard
                        ,ActiveDeck & actingDeck
                        ,CardPosition position
                        ,ActiveDeck & oppositeDeck
                        ,BattleGroundEffect battleGroundEffect
                        ,bool isChaosed
                        ,bool isFusioned
                        ) const;

                    virtual
                    void executeAbilityForOneTarget
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
                        ) const = 0;


                    virtual
                    TargetSet findTargets
                        (EventCondition eventCondition
                        ,ActiveDeck & actingDeck
                        ,CardPosition position
                        ,ActiveDeck & oppositeDeck
                        ,bool isChaosed
                        ) const = 0;
            };

        }
    }
#endif
