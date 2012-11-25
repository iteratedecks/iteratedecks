#ifndef ITERATEDECKS_CORE_DECKINDEX_FORWARD_HPP
    #define ITERATEDECKS_CORE_DECKINDEX_FORWARD_HPP

    #include "simpleTypes.hpp"

    namespace IterateDecks {
        namespace Core {

            struct DeckIndex;
            typedef std::map<DeckIndex, VSTRINGS> MDECKS;
            typedef std::pair<DeckIndex, VSTRINGS> PAIRMDECKS;

        }
    }
#endif
