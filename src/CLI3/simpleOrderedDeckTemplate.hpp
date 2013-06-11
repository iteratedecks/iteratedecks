#ifndef ITERATEDECKS_CLI3_SIMPLEORDEREDDECKTEMPLATE_HPP
    #define ITERATEDECKS_CLI3_SIMPLEORDEREDDECKTEMPLATE_HPP

    #include <memory>
    #include <list>
    #include "../CORE/deckTemplate.hpp"

    namespace C = IterateDecks::Core;
    namespace IterateDecks {
        namespace CLI3 {

            class SimpleOrderedDeckTemplate : public IterateDecks::Core::DeckTemplate {
                public:
                    typedef std::shared_ptr<SimpleOrderedDeckTemplate> Ptr;
                private:
                    unsigned int commander;
                    std::list<unsigned int> cards;
                public:
                    SimpleOrderedDeckTemplate(std::list<unsigned int> const & ids);
                    virtual C::ActiveDeck instantiate(C::CardDB const &) const;
                    virtual std::string toString() const;

                    virtual C::DeckTemplate::Ptr withCommander(unsigned int commanderId) const;
                    virtual size_t getNumberOfNonCommanderCards() const;
                    virtual unsigned int getCardAtIndex(size_t index) const;
                    virtual C::DeckTemplate::Ptr withCardAtIndex(unsigned int cardId, size_t index) const;
                    virtual C::DeckTemplate::Ptr withoutCardAtIndex(size_t index) const;
                    virtual unsigned int getCommander() const;
                    virtual C::DeckTemplate::Ptr swapCards(size_t i, size_t j) const;

                friend bool operator< (SimpleOrderedDeckTemplate const & a, SimpleOrderedDeckTemplate const & b);                    
            };

            bool operator< (SimpleOrderedDeckTemplate const & a, SimpleOrderedDeckTemplate const & b);
        }
    }

#endif
