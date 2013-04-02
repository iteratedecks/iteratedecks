#ifndef ITERATEDECKS_CORE_ITERATEDECKSCORE_HPP
    #define ITERATEDECKS_CORE_ITERATEDECKSCORE_HPP

    #include "deckTemplate.hpp"
    #include "logger.forward.hpp"
    #include "cardDB.hpp"

    // TODO Move that
    #include "../CLI2/cliOptions.hpp"
    using namespace IterateDecks::CLI;

    namespace IterateDecks {
        namespace Core {

            // These ones probably belongs somewhere else
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

            struct SimulationTaskStruct {
                unsigned long minimalNumberOfGames;
                bool surge;
                bool delayFirstAttacker; //< live battles
                BattleGroundEffect battleGround; //< for quests and conquest
                AchievementOptions achievementOptions;
                unsigned int randomSeed;
                bool allowInvalidDecks;
                
                DeckArgument attacker;
                DeckArgument defender;
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

                    SimulationTaskClass(SimulationTaskStruct const & simple, CardDB const & cardDB);
            };
            
            class SimulatorCore {
                public:
                    virtual ~SimulatorCore();
                    virtual Result simulate(SimulationTaskStruct const &) = 0;
                    virtual void abort() = 0;
            };

            class IterateDecksCore : public SimulatorCore {                
                private:
                    unsigned long loggingFlags;
                    bool aborted;
                    CardDB cardDB;

                public:
                    SimulationLogger * logger;

                public:
                    IterateDecksCore();
                    Result simulate(SimulationTaskStruct const &);
                    Result simulate(SimulationTaskClass const &);
                    void abort();

                private:
                    void simulateOnce(SimulationTaskClass const &, Result &);
            };

        }
    }

#endif
