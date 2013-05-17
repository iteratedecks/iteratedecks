#ifndef ITERATEDECKS_CORE_DECKTEMPLATE_HPP
    #define ITERATEDECKS_CORE_DECKTEMPLATE_HPP

    #include <string>
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
                public:
                    typedef std::shared_ptr<DeckTemplate> Ptr;
                    bool allowInvalid;

                protected:
                    DeckTemplate();

                public:
                    virtual ~DeckTemplate();
                    virtual ActiveDeck instantiate(CardDB const & cardDB) const = 0;
                    virtual std::string toString() const = 0;
            };

        }
    }
    
#endif
