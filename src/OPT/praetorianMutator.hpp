#ifndef ITERATEDECKS_OPT_PRAETORIANMUTATOR_HPP
    #define ITERATEDECKS_OPT_PRAETORIANMUTATOR_HPP

    #include <set>
    #include "mutator.hpp"

    #include "../CORE/autoDeckTemplate.hpp"
    #include "../CLI3/simpleOrderedDeckTemplate.hpp"

    #include "derefCompareLT.hpp"

    namespace IterateDecks {
        namespace Opt {

            typedef std::set<unsigned int> CardSet;
            typedef std::multiset<unsigned int> CardMSet;
            typedef std::list<unsigned int> CardList;

            class PraetorianMutator : public Mutator {
                private:
                    bool addCards;
                    bool replaceCards;
                    bool removeCards;
                    bool order;
                    bool fullOrder;
                    bool unorder;
                    bool changeCommander;
                    bool onlyAutoDecks;

                    unsigned int ignoreCardLimitUpTo;
                    CardMSet allowedCards;
                    CardSet allowedCommanders;
                    CardSet allowedNonCommanderCards;

                    CardDB const & cardDB;

                    bool aborted = false;
                    
                public:
                    typedef std::shared_ptr<PraetorianMutator> Ptr;

                private:
                    bool isValid(DeckTemplate::Ptr deck) const;
                    void addChangeCommanderMutations(DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addRemoveMutations         (DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addAddMutations            (DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addReplaceMutations        (DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addSwapMutations           (DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addOrderMutations          (DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addUnorderMutations        (DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    bool canCompose(DeckTemplate::Ptr const & sub) const;
                    template <class InputIterator> void initAllowedCardsFromCollection(InputIterator begin, InputIterator const & end);
                    void initAllowedCardsFromCardDB();
                    bool isExcluded(unsigned int cardId) const;

                public:
                    PraetorianMutator(CardDB const & cardDB);
                    PraetorianMutator(CardDB const & cardDB, CardMSet const & ownedCards, unsigned int ignoreCardLimitUpTo);
                    virtual DeckSet mutate(DeckTemplate::Ptr const & initial);
                    virtual void abort();                    
            };

        }
    }

    #include "praetorianMutator.tpl.hpp"

#endif


