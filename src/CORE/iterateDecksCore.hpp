#ifndef ITERATEDECKS_CORE_ITERATEDECKSCORE_HPP
    #define ITERATEDECKS_CORE_ITERATEDECKSCORE_HPP

    #include <memory>
    #include "deckTemplate.hpp"
    #include "logger.forward.hpp"
    #include "cardDB.hpp"

    // TODO Move that
    //#include "../CLI2/cliOptions.hpp"
    #include "achievementOptions.hpp"
    //using namespace IterateDecks::CLI;

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

                Result();

                Result & operator+=(Result const & rhs);

                double getWinRate() const;
            };

            Result const operator+(Result const & lhs, Result const & rhs);

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

                public:
                    SimulationTaskClass();
            };

            class SimulatorCore {
                public:
                    typedef std::shared_ptr<SimulatorCore> Ptr;

                public:
                    virtual ~SimulatorCore();
                    virtual Result simulate(SimulationTaskClass const &) = 0;
                    virtual void abort() = 0;

                    virtual std::string getCoreName() const = 0;
                    virtual std::string getCoreVersion() const = 0;
                    virtual std::map<std::string,std::string> getXMLVersions() const;
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
                    typedef std::shared_ptr<IterateDecksCore> Ptr;

                public:
                    IterateDecksCore(Core::CardDB const & cardDB);
                    virtual ~IterateDecksCore();
                    void setLogger(Logger * loggerDelegate);
                    Result simulate(SimulationTaskClass const &);
                    void abort();

                    std::string getCoreName() const;
                    std::string getCoreVersion() const;

                    CardDB const & getCardDB() const;
                private:
                    void simulateOnce(SimulationTaskClass const &, Result &, DeckLogger * attackLogger, DeckLogger * defenseLogger);
            };

        }
    }

#endif
