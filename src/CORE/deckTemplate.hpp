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

                    virtual DeckTemplate::Ptr withCommander(unsigned int commanderId) const = 0;
                    virtual size_t getNumberOfNonCommanderCards() const = 0;
                    virtual unsigned int getCardAtIndex(size_t index) const = 0;
                    virtual DeckTemplate::Ptr withCard(unsigned int cardId) const;
                    virtual DeckTemplate::Ptr withCardAtIndex(unsigned int cardId, size_t index) const = 0;
                    virtual DeckTemplate::Ptr withoutCardAtIndex(size_t index) const = 0;
                    virtual unsigned int getCommander() const = 0;
                    //virtual bool isOrdered() const = 0;
                    //virtual DeckTemplate::Ptr asOrdered() const = 0;
                    //virtual DeckTemplate::Ptr asUnordered() const = 0;
                    virtual DeckTemplate::Ptr replaceCardAtIndex(unsigned int cardId, size_t index) const;
                    virtual DeckTemplate::Ptr swapCards(size_t i, size_t j) const = 0;
            };

        }
    }
    
#endif
