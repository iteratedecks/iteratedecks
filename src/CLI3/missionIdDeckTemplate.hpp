#ifndef ITERATEDECKS_CLI3_MISSIONIDTEMPLATE_HPP
    #define ITERATEDECKS_CLI3_MISSIONIDTEMPLATE_HPP

    #include <memory>
    #include <list>
    #include "../CORE/deckTemplate.hpp"

    namespace IterateDecks {
        namespace CLI3 {

            class MissionIdDeckTemplate : public IterateDecks::Core::DeckTemplate {
                public:
                    typedef std::shared_ptr<MissionIdDeckTemplate> Ptr;
                private:
                    unsigned int missionId;
                public:
                    MissionIdDeckTemplate(unsigned int missionId);
                    virtual IterateDecks::Core::ActiveDeck instantiate(IterateDecks::Core::CardDB const &) const;
                    virtual std::string toString() const;

                    virtual IterateDecks::Core::DeckTemplate::Ptr withCommander(unsigned int commanderId) const;
                    virtual size_t getNumberOfNonCommanderCards() const;
                    virtual unsigned int getCardAtIndex(size_t index) const;
                    virtual IterateDecks::Core::DeckTemplate::Ptr withCardAtIndex(unsigned int cardId, size_t index) const;
                    virtual IterateDecks::Core::DeckTemplate::Ptr withoutCardAtIndex(size_t index) const;
                    virtual unsigned int getCommander() const;
                    virtual IterateDecks::Core::DeckTemplate::Ptr swapCards(size_t i, size_t j) const;
            };
        }
    }

#endif

