#ifndef ITERATEDECKS_CLI_DECKTEMPLATES_HPP
    #define ITERATEDECKS_CLI_DECKTEMPLATES_HPP

    #include <list>
    #include <string>
    #include <set>
    #include "../CORE/deckTemplate.hpp"
    #include "../CORE/cardPool.forward.hpp"
    #include "cliOptions.hpp"

    namespace IterateDecks {
        namespace CLI {

            std::list<unsigned int> hashToId(std::string const & hash);
            
            Core::DeckTemplate::Ptr createDeckTemplate(DeckArgument const & argument, Core::CardDB const & cardDB);

            class OrderedDeckTemplate : public Core::DeckTemplate {
                private:
                    Core::Card const * commander;
                    std::list<Core::Card const *> cards;
                public:
                    OrderedDeckTemplate(DeckArgument const & argument, Core::CardDB const & cardDB);
                    virtual Core::ActiveDeck instantiate(Core::CardDB const & cardDB) const;

            };

            class UnorderedDeckTemplate : public Core::DeckTemplate {
                private:
                    Core::Card const * commander;
                    std::multiset<Core::Card const *> cards;
                public:
                    UnorderedDeckTemplate(DeckArgument const & argument, Core::CardDB const & cardDB);
                    virtual Core::ActiveDeck instantiate(Core::CardDB const & cardDB) const;
            };

            class PooledTemplate : public Core::DeckTemplate {
                private:
                    Core::Card const * commander;
                    std::multiset<Core::Card const *> alwaysInclude;
                    std::multiset<Core::CardPool const *> pools;
                    
                public:
                    PooledTemplate(Core::Card const * commander, Core::VCARDPOOL const & pools, Core::CardDB const & cardDB);
                    PooledTemplate(Core::Card const * commander, Core::VID const & alwaysInclude, Core::VCARDPOOL const & pools, Core::CardDB const & cardDB);
                    static PooledTemplate::Ptr createFromRaidId(unsigned int const raidId, Core::CardDB const & cardDB);

                    virtual Core::ActiveDeck instantiate(Core::CardDB const & cardDB) const;

            };

            class QuestTemplate : public PooledTemplate {
                public:
                    QuestTemplate(Core::Card const * commander, Core::VCARDPOOL const & pools, Core::CardDB const & cardDB);

                    static Core::DeckTemplate::Ptr createFromQuestId(unsigned int const questId, Core::CardDB const & cardDB);
            };
        }
    }
#endif
