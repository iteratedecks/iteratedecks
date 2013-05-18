#include "praetorianMutator.hpp"

namespace IterateDecks {
    namespace Opt {

        PraetorianMutator::PraetorianMutator()
        : addCards(true)
        , replaceCards(true)
        , removeCards(true)
        , order(true)
        , fullOrder(true)
        , unorder(true)
        , changeCommander(true)
        {
        }

        std::set<DeckTemplate::Ptr>
        PraetorianMutator::mutate(DeckTemplate::Ptr const & initial)
        {
            
            throw Exception("Not implemented!");
        }

    }
}
