#ifndef ITERATEDECKS_CLI3_MISSIONIDTEMPLATE_HPP
    #define ITERATEDECKS_CLI3_MISSIONIDTEMPLATE_HPP

    #include <memory>
    #include <list>
    #include "../CORE/deckTemplate.hpp"

    namespace IterateDecks {
        namespace CLI3 {

            class MissionIdDeckTemplate : public DeckTemplate {
                public:
                    typedef std::shared_ptr<MissionIdDeckTemplate> Ptr;
                private:
                    unsigned int missionId;
                public:
                    MissionIdDeckTemplate(unsigned int missionId);
                    virtual ActiveDeck instantiate(CardDB const &) const;
                    virtual std::string toString() const;

                    virtual DeckTemplate::Ptr withCommander(unsigned int commanderId) const;
                    virtual size_t getNumberOfNonCommanderCards() const;
                    virtual unsigned int getCardAtIndex(size_t index) const;
                    virtual DeckTemplate::Ptr withCardAtIndex(unsigned int cardId, size_t index) const;
                    virtual DeckTemplate::Ptr withoutCardAtIndex(size_t index) const;
                    virtual unsigned int getCommander() const;
                    virtual DeckTemplate::Ptr swapCards(size_t i, size_t j) const;
            };
        }
    }

#endif

