#ifndef ITERATEDECKS_OPT_MUTATOR_HPP
    #define ITERATEDECKS_OPT_MUTATOR_HPP

    #include <set>
    #include <memory>
    #include "../CORE/deckTemplate.hpp"
    #include "derefCompareLT.hpp"

    namespace IterateDecks {
        namespace Opt {            

            typedef std::set<DeckTemplate::Ptr, DerefCompareLT> DeckSet;

            class Mutator {
                public:
                    typedef std::shared_ptr<Mutator> Ptr;
                public:
                    virtual DeckSet mutate(DeckTemplate::Ptr const & initial) = 0;
            };

        }
    }

#endif

