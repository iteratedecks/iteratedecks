#ifndef ITERATEDECKS_OPT_MUTATOR_HPP
    #define ITERATEDECKS_OPT_MUTATOR_HPP

    #include <set>
    #include <memory>
    #include "../CORE/deckTemplate.hpp"

    namespace IterateDecks {
        namespace Opt {            

            class Mutator {
                public:
                    typedef std::shared_ptr<Mutator> Ptr;
                public:
                    virtual std::set<DeckTemplate::Ptr> mutate(DeckTemplate::Ptr const & initial) = 0;
            };

        }
    }

#endif

