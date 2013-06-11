#ifndef ITERATEDECKS_OPT_DEREFCOMPARELT_HPP
    #define ITERATEDECKS_OPT_DEREFCOMPARELT_HPP

    #include "../CORE/deckTemplate.hpp"

    namespace C = IterateDecks::Core;
    namespace IterateDecks {
        namespace Opt {

            class DerefCompareLT {
                public:
                    bool operator() (C::DeckTemplate::Ptr const & a, C::DeckTemplate::Ptr const & b) const;
                
                    bool operator() (C::DeckTemplate const & a, C::DeckTemplate const &b) const;
            };
        }
    }
#endif
