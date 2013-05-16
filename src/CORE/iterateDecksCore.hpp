#ifndef ITERATEDECKS_CORE_ITERATEDECKSCORE_HPP
    #define ITERATEDECKS_CORE_ITERATEDECKSCORE_HPP

    #include <memory>
    #include "deckTemplate.hpp"
    #include "logger.forward.hpp"
    #include "cardDB.hpp"

    // TODO Move that
    #include "../CLI2/cliOptions.hpp"
    using namespace IterateDecks::CLI;

    namespace IterateDecks {
        namespace Core {

            // TODO These ones probably belongs somewhere else
            struct Result {
                unsigned long numberOfGames;
                unsigned long gamesWon;
                unsigned long gamesStalled;
                unsigned long gamesLost;

                // Note: This is (intentionally) different from NETRAT's RESULT class.
                // Here we store points by player, rather than by winner/loser.
                unsigned long pointsAttacker;
                unsigned long pointsAttackerAuto;
                unsigned long pointsDefender;     //< hardly meaningful: the only time we have a manual defender is in live pvp where damage does not matter.
                unsigned long pointsDefenderAuto;

                // TODO Achievement?
            };

            class SimulationTaskClass {
                public:
                    unsigned long minimalNumberOfGames;
                    bool surge;
                    bool delayFirstAttacker;
                    BattleGroundEffect battleGround;
                    AchievementOptions achievementOptions;
                    unsigned int randomSeed;

                    DeckTemplate::Ptr attacker;
                    DeckTemplate::Ptr defender;
            };

            class SimulatorCore {
                public:
                    typedef std::shared_ptr<SimulatorCore> Ptr;

                public:
                    virtual ~SimulatorCore();
                    virtual Result simulate(SimulationTaskClass const &) = 0;
                    virtual void abort() = 0;
            };

            class IterateDecksCore : public SimulatorCore {
                private:
                    unsigned long loggingFlags;
                    bool aborted;
                    CardDB cardDB;

                private:
                    Logger * loggerDelegate;
                    SimulationLogger * logger;

                public:
                    IterateDecksCore();
                    virtual ~IterateDecksCore();
                    void setLogger(Logger * loggerDelegate);
                    Result simulate(SimulationTaskClass const &);
                    void abort();

                private:
                    void simulateOnce(SimulationTaskClass const &, Result &, DeckLogger * attackLogger, DeckLogger * defenseLogger);
            };

        }
    }

#endif
