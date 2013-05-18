#ifndef ITERATEDECKS_OPT_PRAETORIANMUTATOR_HPP
    #define ITERATEDECKS_OPT_PRAETORIANMUTATOR_HPP

    #include <set>
    #include "mutator.hpp"

    namespace IterateDecks {
        namespace Opt {

            class PraetorianMutator : public Mutator {
                private:
                    bool addCards;
                    bool replaceCards;
                    bool removeCards;
                    bool order;
                    bool fullOrder;
                    bool unorder;
                    bool changeCommander;
                    std::multiset<unsigned int> allowedCards;

                public:
                    typedef std::shared_ptr<PraetorianMutator> Ptr;
                
                public:
                    PraetorianMutator();
                    virtual std::set<DeckTemplate::Ptr> mutate(DeckTemplate::Ptr const & initial);
            };
        }
    }

#endif


