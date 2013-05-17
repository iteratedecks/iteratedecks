#ifndef ITERATEDECKS_CORE_AUTODECKTEMPLATE_HPP
    #define ITERATEDECKS_CORE_AUTODECKTEMPLATE_HPP

    #include <set>
    #include "deckTemplate.hpp"

    namespace IterateDecks {
        namespace Core {

            class AutoDeckTemplate : public DeckTemplate {
                private:
                    unsigned int commander;
                    std::multiset<unsigned int> cards;
                public:
                    AutoDeckTemplate(std::list<unsigned int> const & ids);
                    virtual ActiveDeck instantiate(CardDB const &) const;
                    virtual std::string toString() const;                    
            };

        }
    }
#endif
