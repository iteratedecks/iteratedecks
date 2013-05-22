#include "derefCompareLT.hpp"
#include "../CORE/autoDeckTemplate.hpp"
#include "../CLI3/simpleOrderedDeckTemplate.hpp"

namespace IterateDecks {
    namespace Opt {

        bool
        DerefCompareLT::operator() (DeckTemplate::Ptr const & a, DeckTemplate::Ptr const & b) const {
            return this->operator()(*a,*b);
        }
            
        bool
        DerefCompareLT::operator() (DeckTemplate const & a, DeckTemplate const &b) const {
            IterateDecks::Core::AutoDeckTemplate const * const aa
                = dynamic_cast<IterateDecks::Core::AutoDeckTemplate const *>(&a);
            if (aa) {
                IterateDecks::Core::AutoDeckTemplate const * const ab
                    = dynamic_cast<IterateDecks::Core::AutoDeckTemplate const *>(&b);
                if (ab != NULL) {
                    return (*aa) < (*ab);
                } else {
                    return true;
                }
            }
            IterateDecks::CLI3::SimpleOrderedDeckTemplate const * const sa
                = dynamic_cast<IterateDecks::CLI3::SimpleOrderedDeckTemplate const *>(&a);
            if (sa != NULL) {
                IterateDecks::CLI3::SimpleOrderedDeckTemplate const * const sb
                    = dynamic_cast<IterateDecks::CLI3::SimpleOrderedDeckTemplate const *>(&b);
                if (sb != NULL) {
                    return (*sa) < (*sb);
                } else {
                    return false;
                }
            } else {
                throw Exception("Can't compare them, sorry.");
            }
        }   

    }
}
