#ifndef ITERATEDECKS_CLI3_SIMPLEORDEREDDECKTEMPLATE_HPP
    #define ITERATEDECKS_CLI3_SIMPLEORDEREDDECKTEMPLATE_HPP

    #include <list>
    #include "../CORE/deckTemplate.hpp"

    namespace IterateDecks {
        namespace CLI3 {

            class SimpleOrderedDeckTemplate : public DeckTemplate {
                private:
                    unsigned int commander;
                    std::list<unsigned int> cards;
                public:
                    SimpleOrderedDeckTemplate(std::list<unsigned int> const & ids);
                    virtual ActiveDeck instantiate(CardDB const &) const;
                    virtual std::string toString() const;                    
            };
        }
    }
    
#endif
