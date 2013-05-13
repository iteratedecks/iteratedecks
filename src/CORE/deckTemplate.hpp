#ifndef ITERATEDECKS_CORE_DECKTEMPLATE_HPP
    #define ITERATEDECKS_CORE_DECKTEMPLATE_HPP

    #include <memory>
    #include "simpleTypes.hpp"
    #include "activeDeck.forward.hpp"
    #include "cardDB.forward.hpp"

    // TODO remove that
    #include "../CLI2/cliOptions.hpp"
    using namespace IterateDecks::CLI;

    namespace IterateDecks {
        namespace Core {

            class DeckTemplate {
                protected:
                    Card const * commander;
                    CardDB const & cardDB;
                    
                public:
                    typedef std::shared_ptr<DeckTemplate> Ptr;
                    bool allowInvalid;

                protected:
                    DeckTemplate(CardDB const & cardDB);
                    DeckTemplate(Card const * commander, CardDB const & cardDB);

                public:
                    virtual ~DeckTemplate();
                    virtual ActiveDeck instantiate() const = 0;
            };

        }
    }
    
#endif
