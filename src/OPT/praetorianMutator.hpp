#ifndef ITERATEDECKS_OPT_PRAETORIANMUTATOR_HPP
    #define ITERATEDECKS_OPT_PRAETORIANMUTATOR_HPP

    #include <set>
    #include "mutator.hpp"

    #include "../CORE/autoDeckTemplate.hpp"
    #include "../CLI3/simpleOrderedDeckTemplate.hpp"

    #include "derefCompareLT.hpp"

    namespace IterateDecks {
        namespace Opt {

            namespace C = IterateDecks::Core;

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

                    IterateDecks::Core::CardDB const & cardDB;

                    bool aborted = false;
                    
                public:
                    typedef std::shared_ptr<PraetorianMutator> Ptr;

                private:
                    bool isValid(C::DeckTemplate::Ptr deck) const;
                    void addChangeCommanderMutations(C::DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addRemoveMutations         (C::DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addAddMutations            (C::DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addReplaceMutations        (C::DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addSwapMutations           (C::DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addOrderMutations          (C::DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    void addUnorderMutations        (C::DeckTemplate::Ptr const & original, DeckSet & mutations) const;
                    bool canCompose(C::DeckTemplate::Ptr const & sub) const;
                    template <class InputIterator> void initAllowedCardsFromCollection(InputIterator begin, InputIterator const & end);
                    void initAllowedCardsFromCardDB();
                    bool isExcluded(unsigned int cardId) const;

                public:
                    PraetorianMutator(C::CardDB const & cardDB);
                    PraetorianMutator(C::CardDB const & cardDB, CardMSet const & ownedCards, unsigned int ignoreCardLimitUpTo);
                    virtual DeckSet mutate(C::DeckTemplate::Ptr const & initial);
                    virtual void abort();
                    virtual ~PraetorianMutator() {};
            };

        }
    }

    #include "praetorianMutator.tpl.hpp"

#endif


