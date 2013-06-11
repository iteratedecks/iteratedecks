#ifndef ITERATEDECKS_CORE_AUTODECKTEMPLATE_HPP
    #define ITERATEDECKS_CORE_AUTODECKTEMPLATE_HPP

    #include <memory>
    #include <set>
    #include <list>
    #include "deckTemplate.hpp"

    namespace IterateDecks {
        namespace Core {

            class AutoDeckTemplate : public DeckTemplate {
                public:
                    typedef std::shared_ptr<AutoDeckTemplate> Ptr;
                private:
                    unsigned int commander;
                    std::multiset<unsigned int> cards;
                public:
                    AutoDeckTemplate(std::list<unsigned int> const & ids);
                    virtual ActiveDeck instantiate(CardDB const &) const;
                    virtual std::string toString() const;

                    virtual DeckTemplate::Ptr withCommander(unsigned int commanderId) const;
                    virtual size_t getNumberOfNonCommanderCards() const;
                    virtual unsigned int getCardAtIndex(size_t index) const;
                    virtual DeckTemplate::Ptr withCardAtIndex(unsigned int cardId, size_t index) const;
                    virtual DeckTemplate::Ptr withoutCardAtIndex(size_t index) const;
                    virtual unsigned int getCommander() const;
                    virtual DeckTemplate::Ptr swapCards(size_t i, size_t j) const;

                friend bool operator< (AutoDeckTemplate const & a, AutoDeckTemplate const & b);
            };

            bool operator< (AutoDeckTemplate const & a, AutoDeckTemplate const & b);

        }
    }
#endif
