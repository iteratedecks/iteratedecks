#ifndef ITERATEDECKS_CORE_TARGETMANAGEMENT_HPP
    #define ITERATEDECKS_CORE_TARGETMANAGEMENT_HPP

    #include "playedCard.forward.hpp"
    #include "constants.hpp"
    #include "simpleTypes.hpp"

    namespace IterateDecks {
        namespace Core {

            class Target {
                public:
                    PlayedCard & card;
                    CardPosition position;

                public:
                    Target(PlayedCard & card, CardPosition position);
            };

            typedef std::set<Target> TargetSet;

            /**
             * Accumulates targets into a set.
             */
            void
            getTargets
                (TargetSet & targets //< the set the targets will be added to
                ,std::list<PlayedCard> & cards
                );

            /**
             * Filters targets with an effect from a set.
             * Note that for most cases you should use more specific methods like
             * - removeTargetsThatCannotUseAbilities
             * - removeTargetsThatCannotAttack
             */
            void
            removeTargetsWithEffect
                (TargetSet & targets //< the set to filter
                ,AbilityEnum const & effect //< targets with this effect are removed
                );

            void
            removeTargetsThatCannotAct
                (TargetSet & targets
                );

            void
            removeTargetsThatCannotAttack
                (TargetSet & targets
                );

            /**
             * Filters targets without a faction from a set.
             */
            void
            filterTargetsByFaction
                (TargetSet & targets //< the set to filter
                ,Faction const & faction //<  required faction, maybe none to allow all
                );

            /**
             * Removes targets that started attacking.
             */
            void
            removeTargetsThatStartedAttacking
                (TargetSet & targets //< the set to filter
                );

            /**
             * Filters targets by delay.
             */
            void
            removeTargetsWithDelayOutsideOf
                (TargetSet & targets //< the set to filter
                ,unsigned char minDelay
                ,unsigned char maxDelay
                );

            bool
            isInTargets
                (TargetSet const & targets
                ,PlayedCard const & card
                );

            void
            pickRandomTargetsWithoutIntercept
                (TargetSet & targets
                ,TargetsCount const & targetCount
                );

            void
            removeTargetsThatCannotBeCleansed
                (TargetSet & targets
                );

            void
            removeTargetsLeftOfOrAtSamePosition
                (TargetSet & targets
                ,CardPosition position
                );

        }
    }
#endif
