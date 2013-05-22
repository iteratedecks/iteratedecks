#ifndef ITERATEDECKS_OPT_DEREFCOMPARELT_HPP
    #define ITERATEDECKS_OPT_DEREFCOMPARELT_HPP

    #include "../CORE/deckTemplate.hpp"

    namespace IterateDecks {
        namespace Opt {

            class DerefCompareLT {
                public:
                    bool operator() (DeckTemplate::Ptr const & a, DeckTemplate::Ptr const & b) const;
                
                    bool operator() (DeckTemplate const & a, DeckTemplate const &b) const;
            };
        }
    }
#endif
