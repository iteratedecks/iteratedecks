#ifndef ITERATEDECKS_CLI3_OPTIONPARSER_HPP
    #define ITERATEDECKS_CLI3_OPTIONPARSER_HPP

    #include "../CORE/iterateDecksCore.hpp"
    #include "commands.hpp"

    namespace IterateDecks {
        namespace CLI3 {

            Command::Ptr parseArguments(Core::CardDB const & cardDB, int argc, char const * const * argv);

            IterateDecks::Core::DeckTemplate::Ptr parseDeck(std::string const & deckDescription, Core::CardDB const & cardDB);

        }
    }

#endif
