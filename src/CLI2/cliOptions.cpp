#include "cliOptions.hpp"
#include <iostream>
#include <iomanip>

namespace IterateDecks {
    namespace CLI {

        void printUsage()
        {
            std::cout << "Standard usage: iteratedecks-cli.exe attackhash defensehash" << std::endl;
            std::cout << "Flags:" << std::endl;
            for(unsigned int i = 0; i < numberOfOptions; i++) {
                CommentedOption const & commentedOption(options[i]);
                option const & getOptPart(commentedOption.getOptPart);
                std::string const & comment(commentedOption.comment);
                bool const hasShortOpt(    getOptPart.flag == NULL
                                        && getOptPart.val > 0
                                        && getOptPart.val < 256
                                      );
                if(hasShortOpt) {
                    char const & shortOpt(getOptPart.val);
                    std::cout << "-" << shortOpt;
                } else {
                    std::cout << "  ";
                }
                std::cout << " --";
                std::string const longPart(getOptPart.name);
                std::cout << std::left << std::setw(20) << longPart;
                std::cout << "  ";
                std::cout << comment;
                std::cout << std::endl;
            }
        }

        DeckArgument::DeckArgumentHashData::DeckArgumentHashData()
        : hash()
        , ordered(false)
        {
        }

        DeckArgument::DeckArgument()
        : type(HASH)
        , hashData()
        , missionId(0)
        , raidId(0)
        , questId(0)
        {
        }

        DeckArgument::DeckArgumentType DeckArgument::getType() const
        {
            return this->type;
        }

        std::string DeckArgument::getHash() const throw (InvalidState)
        {
            if(this->type != HASH) {
                throw InvalidState("Only hash decks have a hash.");
            }
            return this->hashData.hash;
        }

        int DeckArgument::getMissionId() const throw (InvalidState)
        {
            if(this->type != MISSION_ID) {
                throw InvalidState("Only raid decks can have a raid id.");
            }
            return this->missionId;
        }

        int DeckArgument::getQuestId() const throw (InvalidState)
        {
            if(this->type != QUEST_ID) {
                throw InvalidState("Only quest decks can have a quest id.");
            }
            return this->questId;
        }

        int DeckArgument::getRaidId() const throw (InvalidState)
        {
            if(this->type != RAID_ID) {
                throw InvalidState("Only raid decks can have a raid id.");
            }
            return this->raidId;
        }

        bool DeckArgument::isOrdered() const throw (InvalidState)
        {
            if(this->type != HASH) {
                throw InvalidState("Only hash decks can be ordered.");
            }
            return this->hashData.ordered;
        }

        void DeckArgument::setOrdered(bool const & ordered)
        {
            if(this->type != HASH) {
                throw InvalidState("Only hash decks can be ordered.");
            }
            this->hashData.ordered = ordered;
        }

        void DeckArgument::setHash(std::string const & hash)
        {
            this->type = HASH;
            this->hashData.hash = hash;
        }

        void DeckArgument::setMission(int const & missionId)
        {
            this->type = MISSION_ID;
            this->missionId = missionId;
        }

        void DeckArgument::setQuest(int const & questId)
        {
            this->type = QUEST_ID;
            this->questId = questId;
        }

        void DeckArgument::setRaid(int const & raidId)
        {
            this->type = RAID_ID;
            this->raidId = raidId;
        }

        CliOptions::CliOptions()
        : numberOfIterations(DEFAULT_NUMBER_OF_ITERATIONS)
        , verbosity(0)
        , seed(1)
        , colorMode(Logger::COLOR_NONE)
        , printHelpAndExit(false)
        , printVersion(false)
        , surge(false)
        , battleGroundEffect(BattleGroundEffect::normal)
        , allowInvalidDecks(false)
        {
        }
    } // namespace CLI
} // namespace EvaluateDecks
