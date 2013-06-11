#include "iterateDecksCore.hpp"
#include "activeDeck.hpp"
#include "assert.hpp"
#include "../VERSION.h"

#include <iostream>
#include <cstdlib>
#include <openssl/md5.h>
#include <fstream>
#include <map>
#include <string>
#include "Logger.hpp"

namespace IterateDecks {
    namespace Core {

        Result::Result()
        : numberOfGames(0)
        , gamesWon(0)
        , gamesStalled(0)
        , gamesLost(0)
        , pointsAttacker(0)
        , pointsAttackerAuto(0)
        , pointsDefender(0)
        , pointsDefenderAuto(0)
        {
        }

        Result &
        Result::operator+=(Result const & rhs)
        {
            this->numberOfGames      += rhs.numberOfGames;
            this->gamesWon           += rhs.gamesWon;
            this->gamesStalled       += rhs.gamesStalled;
            this->gamesLost          += rhs.gamesLost;
            this->pointsAttacker     += rhs.pointsAttacker;
            this->pointsAttackerAuto += rhs.pointsAttackerAuto;
            this->pointsDefender     += rhs.pointsDefender;
            this->pointsDefenderAuto += rhs.pointsDefenderAuto;
            return *this;
        }

        Result const
        operator+(Result const & lhs
                 ,Result const & rhs
                 )
        {
            Result tmp(lhs);
            tmp += rhs;
            return tmp;
        }

        double
        Result::getWinRate() const
        {
            return static_cast<double>(this->gamesWon) / static_cast<double>(this->numberOfGames);
        }

        SimulationTaskClass::SimulationTaskClass()
        : minimalNumberOfGames(1000)
        , surge(false)
        , delayFirstAttacker(false)
        , battleGround(BattleGroundEffect::normal)
        , achievementOptions()
        , randomSeed(0)
        {
        }

        SimulatorCore::~SimulatorCore() {}

        void
        hashFile(std::map<std::string,std::string> & hashes
                ,std::string fileName
                )
        {
            unsigned char digest[MD5_DIGEST_LENGTH];
            char buffer[1024];
            MD5_CTX md5Context;
            MD5_Init(&md5Context);

            std::ifstream file(fileName);
            assertX(file.is_open());
            while(!(file.eof())) {
                //std::streamsize n = file.readsome(buffer, sizeof(buffer));
                file.read(buffer, sizeof(buffer));
                std::streamsize n = file.gcount();
                MD5_Update(&md5Context, buffer, n);
            }
            file.close();
            MD5_Final(digest, &md5Context);
            std::stringstream ssDigest;
            for(int i = 0; i < MD5_DIGEST_LENGTH; i++) {
                ssDigest << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)(digest[i]);
            }
            hashes[fileName] = ssDigest.str();
        }

        std::map<std::string,std::string> SimulatorCore::getXMLVersions() const
        {
            std::map<std::string,std::string> hashes;
            hashFile(hashes, "achievements.xml");
            hashFile(hashes, "cards.xml");
            hashFile(hashes, "missions.xml");
            hashFile(hashes, "raids.xml");
            hashFile(hashes, "quests.xml");
            return hashes;
        }

        IterateDecksCore::IterateDecksCore()
        : logger(NULL)
        , loggerDelegate(NULL)
        {
            this->cardDB.LoadAchievementXML("achievements.xml");
            this->cardDB.LoadCardXML("cards.xml");
            this->cardDB.LoadMissionXML("missions.xml");
            this->cardDB.LoadRaidXML("raids.xml");
            this->cardDB.LoadQuestXML("quests.xml");
        }

        void IterateDecksCore::setLogger(Logger * loggerDelegate) {
            if (this->logger != NULL) {
                delete this->logger;
                this->logger = NULL;
            }
            this->loggerDelegate = loggerDelegate;
            if (this->loggerDelegate) {
                this->logger = new SimulationLogger(*this->loggerDelegate);
            }
        }

        IterateDecksCore::~IterateDecksCore() {
            if (this->logger != NULL) {
                delete this->logger;
                this->logger = NULL;
            }
        }

        Result IterateDecksCore::simulate(SimulationTaskClass const & task) {
            // Store results
            Result result;

            // Setup seed
            std::srand(task.randomSeed);

            // Logging?
            DeckLogger * attackLogger  = this->loggerDelegate ? new DeckLogger(DeckLogger::ATTACK , *(this->loggerDelegate)) : NULL;
            DeckLogger * defenseLogger = this->loggerDelegate ? new DeckLogger(DeckLogger::DEFENSE, *(this->loggerDelegate)) : NULL;

            unsigned int const n = task.minimalNumberOfGames;
            this->aborted = false;
            for(unsigned int i = 0; i < n; i++) {
                LOG(this->logger,simulationStart(i));
                this->simulateOnce(task, result, attackLogger, defenseLogger);
                LOG(this->logger,simulationEnd(i));

                // aborted?
                if (this->aborted) {
                    break;
                }
            }

            delete attackLogger;
            delete defenseLogger;

            return result;
        }

        void IterateDecksCore::abort() {
            this->aborted = true;
        }

        void IterateDecksCore::simulateOnce(SimulationTaskClass const & task
                                           ,Result & result
                                           ,DeckLogger * attackLogger
                                           ,DeckLogger * defenseLogger
                                           )
        {
            // We play one game.
            // 1. What about the battleground ?
            BattleGroundEffect questEffect = task.battleGround;

            // 2. We need the decks
            ActiveDeck attacker = task.attacker->instantiate(this->cardDB);
            attacker.SetQuestEffect(questEffect);
            attacker.logger = attackLogger;
            ActiveDeck defender = task.defender->instantiate(this->cardDB);
            defender.SetQuestEffect(questEffect);
            defender.logger = defenseLogger;

            // Surge or not?
            bool const surge = task.surge;
            bool defendersTurn = surge;

            unsigned int attAutoDamageToCommander = 0;
            unsigned int defAutoDamageToCommander = 0;
            unsigned int lastManualTurn = 1;
            unsigned int i;
            for(i = 1; i <= 50; i++) {

                // reset damage done and update last manual turn counter
                // P: I'm not sure whether I like this.
                //    This does not allow us to do "start on manual, then go auto" style
                if (attacker.Deck.size() >= 2) {
                    attAutoDamageToCommander += attacker.DamageToCommander;
                    defAutoDamageToCommander += defender.DamageToCommander;
                    attacker.DamageToCommander = 0;
                    defender.DamageToCommander = 0;
                    lastManualTurn = i;
                }

                // Attack
                LOG(this->logger,turnStart(i));
                if(defendersTurn) {
                    defender.AttackDeck(attacker, false, i);
                } else {
                    attacker.AttackDeck(defender, false, i);
                }
                LOG(this->logger,turnEnd(i));

                // One might be dead
                bool const attackerAlive = attacker.isAlive();
                bool const defenderAlive = defender.isAlive();

                // At least one should be still alive.
                // P: I can not think of many situations where you can actually lose in your turn.
                //    Only thing that comes to mind being backfire.
                // P: Or a chaosed shock.
                assertX(attackerAlive || defenderAlive);
                bool const gameOver = !(attackerAlive && defenderAlive);

                if(gameOver) {
                    break;
                }

                // swap whose turn it is
                defendersTurn = !defendersTurn;
            } // for

            unsigned int const attManualDamageToCommander = attacker.DamageToCommander;
            unsigned int const defManualDamageToCommander = defender.DamageToCommander;
            attAutoDamageToCommander += attManualDamageToCommander;
            defAutoDamageToCommander += defManualDamageToCommander;
            bool const attackerAlive = attacker.isAlive();
            bool const defenderAlive = defender.isAlive();
            assertX(attackerAlive || defenderAlive);

            if(!attackerAlive) {
                // defender won
                result.gamesLost++;
                result.pointsDefender     += 10u;
                result.pointsDefenderAuto += 10u;
                // time bonus?
                if(i < lastManualTurn + 10) {
                    LOG(this->logger,scoreTime(false, false, 5u));
                    result.pointsDefender     += 5u;
                }
                if(i <= 10) {
                    LOG(this->logger,scoreTime(false, true, 5u));
                    result.pointsDefenderAuto += 5u;
                }
            } else if(!defenderAlive) {
                // attacker won
                //std::clog << "awarding points to attacker for winning" << std::endl;
                result.gamesWon++;
                result.pointsAttacker     += surge ? 30u : 10u;
                result.pointsAttackerAuto += surge ? 30u : 10u;
                // time bonus?
                if(i < lastManualTurn + 10) {
                    LOG(this->logger,scoreTime(true, false, 5u));
                    result.pointsAttacker     += 5u;
                }
                if(i <= 10) {
                    LOG(this->logger,scoreTime(true, true, 5u));
                    result.pointsAttackerAuto += 5u;
                }
            } else {
                // both alive
                result.gamesStalled++;
                result.pointsDefender     += 10u;
                result.pointsDefenderAuto += 10u;
                // certainly no time bonus on stalls
            }

            // damage
            //std::clog << "damage for attacker. "
            //          << "manual=" << attManualDamageToCommander << " "
            //          << "auto=" << attAutoDamageToCommander << std::endl;
            unsigned int const attManualDamagePoints = std::min(attManualDamageToCommander, 10u);
            unsigned int const attAutoDamagePoints = std::min(attAutoDamageToCommander, 10u);
            unsigned int const defManualDamagePoints = std::min(defManualDamageToCommander, 10u);
            unsigned int const defAutoDamagePoints = std::min(defAutoDamageToCommander, 10u);
            LOG(this->logger, scoreDamage(true , false, attManualDamagePoints));
            LOG(this->logger, scoreDamage(true , true , attAutoDamagePoints));
            LOG(this->logger, scoreDamage(false, false, defManualDamagePoints));
            LOG(this->logger, scoreDamage(false, true , defAutoDamagePoints));
            result.pointsAttacker     += attManualDamagePoints;
            result.pointsAttackerAuto += attAutoDamagePoints;
            result.pointsDefender     += defManualDamagePoints;
            result.pointsDefenderAuto += defAutoDamagePoints;

            // also increase number of games
            result.numberOfGames++;
        }

        std::string IterateDecksCore::getCoreName() const
        {
            return "IterateDecks";
        }

        std::string IterateDecksCore::getCoreVersion() const
        {
            std::stringstream ssVersion;
            ssVersion << ITERATEDECKS_VERSION;
            if(ITERATEDECKS_DIRTY_HEAD_CORE) {
                ssVersion << "+";
                ssVersion << ITERATEDECKS_DIRTY_HASH_CORE;
            }
            return ssVersion.str();
        }

        CardDB const &
        IterateDecksCore::getCardDB() const
        {
            return this->cardDB;
        }


    }
}
