#include "cliOptions.hpp"
#include <iostream>
#include <iomanip>

namespace EvaluateDecks {
    namespace CLI {

        //CommentedOption const options[] =
        //		{ { { "number-of-iterations" , required_argument, 0, 'n' }, "sets the number of simulations to do" }
        //		, { { "first-deck-is-ordered", no_argument      , 0, 'o' }, "marks the first deck (player deck) as ordered" }
        //		, { { "achievement-index"    , required_argument, 0, 'a' }, "index (not id) of achievement. not sure where the difference is" }
        //		, { { "verify"               , required_argument, 0, 0   }, "verify a result, provide an accepted range in the form <lower bound>:<upper bound>, like \"--verify 1:1\" if the deck should win all the time" }
        //		, { { "verbose"              , no_argument      , 0, 'v' }, "verbose output" }
        //		, { { "seed"                 , optional_argument, 0, 0   }, "set the seed, takes an optional argument. if none given use seed based on time." }
        //		, { { "color"                , optional_argument, 0, 0   }, "color the output, currently only ANSI colors supported" }
        //		, { { "help"                 , no_argument      , 0, 'h' }, "print help" }
        //		};

        //size_t const numberOfOptions(sizeof(options)/sizeof(CommentedOption));

        void printUsage()
        {
            std::cout << "Usage:" << std::endl;
            for(unsigned int i = 0; i < numberOfOptions; i++) {
                CommentedOption const & commentedOption(options[i]);
                option const & getOptPart(commentedOption.getOptPart);
                std::string const & comment(commentedOption.comment);
                bool const hasShortOpt(getOptPart.flag == NULL && getOptPart.val != 0);
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
                throw InvalidState("Only raid decks can have a raid id.");
            if(this->type != QUEST_ID) {
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

        AchievementOptions::AchievementOptions()
        : doAchievementCheck(false)
        , achievementIndex(0)
        {
        }

        void AchievementOptions::enableCheck(unsigned int const & achievementIndex)
        {
            this->doAchievementCheck = true;
            this->achievementIndex = achievementIndex;
        }

        void AchievementOptions::disableCheck()
        {
            this->doAchievementCheck = false;
            this->achievementIndex = 0;
        }

        AchievementOptions::operator int() const
        {
            if(this->doAchievementCheck) {
                return this->achievementIndex;
            } else {
                return -1;
            }
        }

        CliOptions::CliOptions()
        : numberOfIterations(DEFAULT_NUMBER_OF_ITERATIONS)
        , verbosity(0)
        , seed(1)
        , colorMode(Logger::COLOR_NONE)
        , printHelpAndExit(false)
        , surge(false)
        {
        }
    } // namespace CLI
} // namespace EvaluateDecks
