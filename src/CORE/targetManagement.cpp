#include "targetManagement.hpp"
#include "simpleTypes.hpp"
#include "playedCard.hpp"

namespace IterateDecks {
    namespace Core {

        Target::Target(PlayedCard & card
                      ,CardPosition position
                      )
        : card(card)
        , position(position)
        {
        }

        bool operator< (Target const & a, Target const b) {
            return a.position < b.position;
        }

        void
        getTargets(TargetSet & targets
                  ,std::list<PlayedCard> & cards
                  )
        {
            unsigned int i = 0;
            for(std::list<PlayedCard>::iterator iter = cards.begin()
               ;iter != cards.end()
               ;iter++, i++
               )
            {
                // FIXME that feels ugly with the position
                Target target(*iter,i);
                
                targets.insert(target);
            }
        }

        void
        removeTargetsWithEffect(TargetSet & targets
                               ,Abilities const & effect
                               )
        {
            TargetSet::iterator iter = targets.begin();
            while(iter != targets.end()) {
                if (iter->card.GetEffect(effect) > 0) {
                    iter = targets.erase(iter);
                } else {
                    iter++;
                }
            }
        }

        void
        removeTargetsThatCannotAttack(TargetSet & targets
                                     )
        {
            removeTargetsWithEffect(targets, ACTIVATION_JAM);
            removeTargetsWithEffect(targets, ACTIVATION_FREEZE);
            removeTargetsWithEffect(targets, DMGDEPENDANT_IMMOBILIZE);
            removeTargetsWithEffect(targets, DEFENSIVE_STUN);
        }

        void
        removeTargetsThatCannotAct(TargetSet & targets
                                     )
        {
            removeTargetsWithEffect(targets, ACTIVATION_JAM);
            removeTargetsWithEffect(targets, ACTIVATION_FREEZE);
        }

    
        void
        filterTargetsByFaction(TargetSet & targets
                              ,Faction const & faction
                              )
        {
            if(faction != FACTION_NONE) {
                TargetSet::iterator iter = targets.begin();
                while(iter != targets.end()) {
                    if (iter->card.GetFaction() != faction) {
                        iter = targets.erase(iter);
                    } else {
                        iter++;
                    }
                }
            }
        }

        void
        removeTargetsThatStartedAttacking(TargetSet & targets
                                         )
        {
            TargetSet::iterator iter = targets.begin();
            while(iter != targets.end()) {
                if (iter->card.getStage() >= CardActionStages::attackStarted) {
                    iter = targets.erase(iter);
                } else {
                    iter++;
                }
            }
        }

        void
        removeTargetsWithDelayOutsideOf(TargetSet & targets
                                       ,unsigned char minDelay
                                       ,unsigned char maxDelay
                                       )
        {
            TargetSet::iterator iter = targets.begin();
            while(iter != targets.end()) {
                DelayType delay = iter->card.GetWait();
                if (delay < minDelay || maxDelay < delay) {
                    iter = targets.erase(iter);
                } else {
                    iter++;
                }
            }
        }

        bool
        isInTargets
            (TargetSet const & targets
            ,PlayedCard const & card
            )
        {
            for(TargetSet::const_iterator iter = targets.begin()
               ;iter != targets.end()
               ;iter++)
            {
                if(&(iter->card) == &card) {
                    return true;
                }
            }
            return false;
        }

        void pickRandomTargetsWithoutIntercept
            (TargetSet & targets
            ,TargetsCount const & targetCount
            )
        {
            switch(targetCount) {
                case TARGETSCOUNT_ALL:
                    break;
                case TARGETSCOUNT_ONE: {
                        size_t count = targets.size();
                        if (count > 0) {
                            // P: I am aware that this is not exactly even distributed. However the error is very small.
                            size_t index = rand() % count;
                            TargetSet::const_iterator iter = targets.begin();
                            std::advance(iter,index);
                            Target selectedOne = *iter;
                            targets.clear();
                            targets.insert(selectedOne);
                        }
                    } break;
                default:
                    throw Exception("Switch case missed");
            }
        }

        void removeTargetsThatCannotBeCleansed(TargetSet & targets
                                              )
        {
            TargetSet::iterator iter = targets.begin();
            while (iter != targets.end()) {
                if (!iter->card.IsCleanseTarget()) {
                    iter = targets.erase(iter);
                } else {
                    iter++;
                }
            }
        }

        void
        removeTargetsLeftOfOrAtSamePosition(TargetSet & targets
                                           ,CardPosition position
                                           )
        {
            TargetSet::iterator iter = targets.begin();
            while (iter != targets.end()) {
                if (!iter->position <= position) {
                    iter = targets.erase(iter);
                } else {
                    iter++;
                }
            }
        }

        void
        removeTargetsWithZeroOrLessAttack
            (TargetSet & targets
            )
        {
            TargetSet::iterator iter = targets.begin();
            while (iter != targets.end()) {
                if (!iter->card.GetAttack() <= 0) {
                    iter = targets.erase(iter);
                } else {
                    iter++;
                }
            }
        }
        
    }
}
