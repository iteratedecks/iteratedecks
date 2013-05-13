#ifndef ITERATEDECKS_CLI_DECKTEMPLATES_HPP
    #define ITERATEDECKS_CLI_DECKTEMPLATES_HPP

    #include <list>
    #include <string>
    #include <set>
    #include "../CORE/deckTemplate.hpp"
    #include "../CORE/cardPool.forward.hpp"

    namespace IterateDecks {
        namespace CLI {

            std::list<unsigned int> hashToId(std::string const & hash);
            
            DeckTemplate::Ptr createDeckTemplate(DeckArgument const & argument, CardDB const & cardDB);


            class OrderedDeckTemplate : public DeckTemplate {
                private:
                    std::list<Card const *> cards;
                public:
                    OrderedDeckTemplate(DeckArgument const & argument, CardDB const & cardDB);
                    virtual ActiveDeck instantiate() const;

            };

            class UnorderedDeckTemplate : public DeckTemplate {
                private:
                    std::multiset<Card const *> cards;
                public:
                    UnorderedDeckTemplate(DeckArgument const & argument, CardDB const & cardDB);
                    virtual ActiveDeck instantiate() const;
            };

            class PooledTemplate : public DeckTemplate {
                private:
                    std::multiset<Card const *> alwaysInclude;
                    std::multiset<CardPool const *> pools;

                public:
                    PooledTemplate(Card const * commander, VCARDPOOL const & pools, CardDB const & cardDB);
                    PooledTemplate(Card const * commander, VID const & alwaysInclude, VCARDPOOL const & pools, CardDB const & cardDB);
                    static PooledTemplate::Ptr createFromRaidId(unsigned int const raidId, CardDB const & cardDB);

                    virtual ActiveDeck instantiate() const;

            };

            class QuestTemplate : public PooledTemplate {
                public:
                    QuestTemplate(Card const * commander, VCARDPOOL const & pools, CardDB const & cardDB);

                    static DeckTemplate::Ptr createFromQuestId(unsigned int const questId, CardDB const & cardDB);
            };
        }
    }
#endif
