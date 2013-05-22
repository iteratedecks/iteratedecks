#include "praetorianMutator.hpp"
#include "../CORE/activeDeck.hpp"
#include "../CORE/constants.hpp"
#include "../CORE/autoDeckTemplate.hpp"
#include "../CLI3/simpleOrderedDeckTemplate.hpp"
#include "../CORE/assert.hpp"
#include "../CORE/cardDB.hpp"
#include <iostream>
#include <iomanip>

namespace IterateDecks {
    namespace Opt {

        PraetorianMutator::PraetorianMutator(CardDB const & cardDB)
        : addCards(true)
        , replaceCards(true)
        , removeCards(true)
        , order(true)
        , fullOrder(true)
        , unorder(true)
        , changeCommander(true)
        , noCardLimit(true)
        , cardDB(cardDB)
        {
            for(size_t cardId = 0; cardId < CARD_MAX_ID; cardId++) {
                Card const & card = cardDB.GetPointer()[cardId];
                if (card.GetType() != TYPE_NONE) {
                    if(card.GetType() == TYPE_COMMANDER) {
                        this->allowedCommanders.insert(cardId);
                    } else {
                        this->allowedNonCommanderCards.insert(cardId);
                    }
                }
            }
        }

        bool
        PraetorianMutator::isValid(DeckTemplate::Ptr deck) const
        {
            return deck->isValid(this->cardDB);
            //return deck->instantiate(this->cardDB).IsValid();
        }

        bool isOrdered(DeckTemplate const & deck)
        {
            // dirty code
            if (dynamic_cast<IterateDecks::Core::AutoDeckTemplate const *>(&deck) != NULL) {
                return false;
            } else if (dynamic_cast<IterateDecks::CLI3::SimpleOrderedDeckTemplate const *>(&deck) != NULL) {
                return true;
            } else {
                assertX(false);
                return true;
            }
        }

        DeckTemplate::Ptr
        asOrdered(DeckTemplate::Ptr orig)
        {
            CardList cards;
            cards.push_back(orig->getCommander());
            for(size_t i = 0; i < orig->getNumberOfNonCommanderCards(); i++) {
                cards.push_back(orig->getCardAtIndex(i));
            }
            return DeckTemplate::Ptr(new IterateDecks::CLI3::SimpleOrderedDeckTemplate(cards));
        }

        DeckTemplate::Ptr
        asUnordered(DeckTemplate::Ptr orig)
        {
            throw Exception("Not implemented!");
        }

        template <class T>
        bool isSubSet(std::multiset<T> const & sub
                     ,std::multiset<T> const & super
                     )
        {
            typedef typename std::multiset<T> SetType;
            typedef typename SetType::const_iterator ConstIteratorType;
            typedef typename SetType::iterator IteratorType;
            SetType copyOfSuper(super);
            for(ConstIteratorType iter = sub.begin()
               ;iter != sub.end()
               ;iter++)
            {
                IteratorType superIter =
                    copyOfSuper.find(*iter);
                if(superIter == copyOfSuper.end()) {
                    return false;
                } else {
                    copyOfSuper.erase(superIter);
                }
            }
            return true;
        }

        bool
        PraetorianMutator::canCompose(DeckTemplate::Ptr const & sub
                                   ) const
        {
            if (this->noCardLimit) {
                return true;
            } else {
                CardMSet sub2;
                sub2.insert(sub->getCommander());
                size_t l = sub->getNumberOfNonCommanderCards();
                for(size_t i = 0; i < l; i++) {
                    sub2.insert(sub->getCardAtIndex(i));
                }
                return isSubSet(sub2, this->allowedCards);
            }
        }

        void
        PraetorianMutator::addChangeCommanderMutations(DeckTemplate::Ptr const & original
                                                      ,DeckSet & mutations
                                                      ) const
        {
            unsigned int count = 0;
            for(CardMSet::const_iterator iter = this->allowedCommanders.begin()
               ;iter != this->allowedCommanders.end()
               ;iter++)
            {
                unsigned int cardId = *iter;
                DeckTemplate::Ptr mutation = original->withCommander(cardId);
                if(isValid(mutation) && canCompose(mutation)) {
                    mutations.insert(mutation);
                    count++;
                }
            }
            std::clog << "\tfound " << std::setw(5) << count << " change commander mutations." << std::endl;
        }

        /**
         * Mutate the given deck by removing one card. Adds all possible
         * variations (i.e, one deck for each card in the original):
         *
         * @param original the original deck
         * @param mutations the set to add the mutations to
         */
        void
        PraetorianMutator::addRemoveMutations(DeckTemplate::Ptr const & original
                                             ,DeckSet & mutations
                                             ) const
        {
            unsigned int count = 0;
            size_t const numberOfCards = original->getNumberOfNonCommanderCards();
            for(size_t i = 0; i < numberOfCards; i++) {
                DeckTemplate::Ptr mutation = original->withoutCardAtIndex(i);
                mutations.insert(mutation);
                count++;
            }
            std::clog << "\tfound " << std::setw(5) << count << " remove card mutations." << std::endl;
        }

        void
        PraetorianMutator::addAddMutations(DeckTemplate::Ptr const & original
                                          ,DeckSet & mutations
                                          ) const
        {
            unsigned int count = 0;
            size_t const numberOfCards = original->getNumberOfNonCommanderCards();
            if (numberOfCards < DEFAULT_DECK_SIZE) {
                // consider all possible cards
                for(CardSet::const_iterator iter = this->allowedNonCommanderCards.begin()
                   ;iter != this->allowedNonCommanderCards.end()
                   ;iter++)
                {
                    unsigned int cardId = *iter;
                    if(!isOrdered(*original)) {
                        DeckTemplate::Ptr mutation = original->withCard(cardId);
                        // check for validity and can compose
                        if(this->isValid(mutation) && this->canCompose(mutation)) {
                            mutations.insert(mutation);
                            count++;
                        }
                    } else {
                        for(unsigned int i = 0; i <= numberOfCards; i++) {
                            DeckTemplate::Ptr mutation = original->withCardAtIndex(cardId, i);
                            if(this->isValid(mutation) && this->canCompose(mutation)) {
                                mutations.insert(mutation);
                                count++;
                            }
                        }
                    } // (un)ordered
                }
            }
            std::clog << "\tfound " << std::setw(5) << count << " add card mutations." << std::endl;
        }

        /**
         * Replace a card.
         */
        void
        PraetorianMutator::addReplaceMutations(DeckTemplate::Ptr const & original
                                              ,DeckSet & mutations
                                              ) const
        {
            unsigned int count = 0;
            size_t const numberOfCards = original->getNumberOfNonCommanderCards();
            for(size_t i = 0; i < numberOfCards; i++) {
                 // consider all possible cards
                for(CardSet::const_iterator iter = this->allowedNonCommanderCards.begin()
                   ;iter != this->allowedNonCommanderCards.end()
                   ;iter++)
                {
                    unsigned int const cardId = *iter;
                    // replace
                    DeckTemplate::Ptr mutation = original->replaceCardAtIndex(cardId,i);

                    // check for validity
                    if(!isValid(mutation)) {
                        //std::clog << "Invalid" << std::endl;
                        //std::clog << mutation << std::endl;
                        //std::clog << "-------" << std::endl;
                        continue;
                    }
                    // can compose?
                    if(!this->canCompose(mutation)) {
                        //std::clog << "Can not compose" << std::endl;
                        //std::clog << mutation << std::endl;
                        //std::clog << "-------" << std::endl;
                        continue;
                    }

                    //std::clog << mutation << std::endl;
                    mutations.insert(mutation);
                    count++;
                } // for
            }
            std::clog << "\tfound " << std::setw(5) << count << " replace card mutations." << std::endl;
        }

        void
        addSwapMutation(DeckTemplate::Ptr const & original
                       ,DeckSet & mutations
                       ,unsigned int const i
                       ,unsigned int const j
                       )
        {
            DeckTemplate::Ptr mutation = original->swapCards(i,j);
            mutations.insert(mutation);
        }

        void
        PraetorianMutator::addSwapMutations(DeckTemplate::Ptr const & original
                                           ,DeckSet & mutations
                                           ) const
        {
            unsigned int count = 0;
            if (isOrdered(*original)) {
                size_t const numberOfCards = original->getNumberOfNonCommanderCards();
                for(unsigned int i = 0; i+1 < numberOfCards; i++) {
                    for(unsigned int j = i+1; j < numberOfCards; j++) {
                         addSwapMutation(original, mutations, i, j);
                         count++;
                    } // for j
                } // for i
            }
            std::clog << "\tfound " << std::setw(5) << count << " swap cards mutations." << std::endl;
        }

        #if 0
        void generatePermutations(CardList const & orderedPart
                                 ,CardMSet const & unorderedPart
                                 ,CardListSet & permutations
                                 )
        {
            if(unorderedPart.empty()) {
                // if there are no more unordered cards just return the ordered part
                permutations.insert(orderedPart);
            } else {
                // need to do permutations of the rest
                PCardSet cards(unorderedPart.begin(), unorderedPart.end());
                assertX(!cards.empty());
                for(PCardSet::const_iterator iter = cards.begin()
                   ;iter != cards.end()
                   ;iter++)
                {
                    PCardList newOrderedPart(orderedPart);
                    newOrderedPart.push_back(*iter);
                    assertX(newOrderedPart.size() == orderedPart.size()+1);
                    PCardMSet newUnorderedPart(unorderedPart);
                    newUnorderedPart.erase(newUnorderedPart.find(*iter));
                    assertX(newUnorderedPart.size()+1 == unorderedPart.size());
                    // recurse
                    generatePermutations(newOrderedPart
                                        ,newUnorderedPart
                                        ,permutations
                                        );
                }
            }
        }
        #endif

        void
        PraetorianMutator::addOrderMutations(DeckTemplate::Ptr const & original
                                            ,DeckSet & mutations
                                            ) const
        {
            unsigned int count = 0;
            if(!isOrdered(*original)) {
                #if 0
                    //std::clog << "ordering..." << std::endl;
                    // compute all orders... thats gonna be a lot
                    PCardList const emptyList;
                    PCardMSet const cards = original.getMCards();
                    PCardListSet permutations;
                    generatePermutations(emptyList, cards, permutations);
                    for(PCardListSet::const_iterator iter = permutations.begin()
                       ;iter != permutations.end()
                       ;iter++)
                    {
                        PCardList permutation = *iter;
                        Deck mutation(original.getCommander(), permutation, true);
                        //std::clog << mutation << std::endl;
                        mutations.insert(mutation);
                        count++;
                    }
                #else
                    DeckTemplate::Ptr mutation = asOrdered(original);
                    mutations.insert(mutation);
                    count++;
                #endif
            }
            std::clog << "\tfound " << std::setw(5) << count << " order deck mutations." << std::endl;
        }

        void
        PraetorianMutator::addUnorderMutations(DeckTemplate::Ptr const & original
                                              ,DeckSet & mutations
                                              ) const
        {
            unsigned int count = 0;
            if(isOrdered(*original)) {
                DeckTemplate::Ptr mutation = asUnordered(original);
                mutations.insert(mutation);
                count++;
            }
            std::clog << "\tfound " << std::setw(5) << count << " unorder deck mutations." << std::endl;
        }

        DeckSet
        PraetorianMutator::mutate(DeckTemplate::Ptr const & initial)
        {
            DeckSet mutations = DeckSet(DerefCompareLT());
            mutations.insert(initial);
            this->addChangeCommanderMutations(initial, mutations);
            this->addRemoveMutations         (initial, mutations);
            this->addAddMutations            (initial, mutations);
            this->addReplaceMutations        (initial, mutations);
            this->addSwapMutations           (initial, mutations);
            this->addOrderMutations          (initial, mutations);
            this->addUnorderMutations        (initial, mutations);
            return mutations;
        }

    }
}
