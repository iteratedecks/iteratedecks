#ifndef ITERATEDECKS_OPT_PRAETORIANMUTATOR_HPP
    #define ITERATEDECKS_OPT_PRAETORIANMUTATOR_HPP

    #include <set>
    #include "mutator.hpp"

    namespace IterateDecks {
        namespace Opt {

            typedef std::set<DeckTemplate::Ptr> DeckSet;
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

                    bool noCardLimit;
                    CardMSet allowedCards;
                    CardSet allowedCommanders;
                    CardSet allowedNonCommanderCards;

                    CardDB const & cardDB;

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

                public:
                    PraetorianMutator(CardDB const & cardDB);
                    virtual std::set<DeckTemplate::Ptr> mutate(DeckTemplate::Ptr const & initial);
            };
        }
    }

#endif


