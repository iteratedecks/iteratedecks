#ifndef ITERATEDECKS_CLI3_DECKPARSER_HPP
#define ITERATEDECKS_CLI3_DECKPARSER_HPP

    #include "../CORE/deckTemplate.hpp"
    #include "../CORE/cardDB.hpp"
    #include <istream>

    namespace IterateDecks {
        namespace CLI3 {
    
            ::IterateDecks::Core::DeckTemplate::Ptr
            parseDeckFromStream(std::istream & is, Core::CardDB const & cardDB);

        }
    }
#endif

