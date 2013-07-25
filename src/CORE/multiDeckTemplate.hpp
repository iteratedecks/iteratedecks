#ifndef ITERATEDECKS_CORE_MULTIDECKTEMPLATE_HPP
    #define ITERATEDECKS_CORE_MULTIDECKTEMPLATE_HPP

    #include <memory>
    #include <set>
    #include "deckTemplate.hpp"

    namespace IterateDecks {
        namespace Core {

            class MultiDeckTemplate : public DeckTemplate {
                public:
                    typedef std::shared_ptr<MultiDeckTemplate> Ptr;
                private:
                    std::multiset<DeckTemplate::Ptr> decks;
                public:
                    MultiDeckTemplate(std::multiset<DeckTemplate::Ptr> const & decks);
                    virtual ActiveDeck instantiate(CardDB const &) const;
                    virtual std::string toString() const;

                    virtual DeckTemplate::Ptr withCommander(unsigned int commanderId) const;
                    virtual size_t getNumberOfNonCommanderCards() const;
                    virtual unsigned int getCardAtIndex(size_t index) const;
                    virtual DeckTemplate::Ptr withCardAtIndex(unsigned int cardId, size_t index) const;
                    virtual DeckTemplate::Ptr withoutCardAtIndex(size_t index) const;
                    virtual unsigned int getCommander() const;
                    virtual DeckTemplate::Ptr swapCards(size_t i, size_t j) const;

                    std::multiset<DeckTemplate::Ptr> const & getDecks() const;
                
            };

        }
    }
#endif

