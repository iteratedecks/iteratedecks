#ifndef CLI_OPTIONS_H_
    #define CLI_OPTIONS_H_

    #include "verify.hpp"
    #include "../EXE/Logger.hpp"
    #include "../EXE/exceptions.hpp"

#ifdef _WIN32
    #define __windows__
#endif
#ifdef _WIN64
    #define __windows__
#endif

#if defined(__windows__)
    #include "../../EvaluateDecks/getopt_mb_uni_vc10/getopt_mb_uni_vc10_dll/getopt.h"
#else
    #include <getopt.h>
#endif

    namespace EvaluateDecks {
        namespace CLI {

            struct CommentedOption {
                option getOptPart;
                std::string comment;
            };

        /**
         * Option table
         */
            static CommentedOption const options[] =
                { { { "number-of-iterations" , required_argument, 0, 'n' }, "sets the number of simulations to do" }
                , { { "first-deck-is-ordered", no_argument      , 0, 'o' }, "marks the first deck (player deck) as ordered" }
                , { { "achievement-index"    , required_argument, 0, 'a' }, "index (not id) of achievement. not sure where the difference is" }
                , { { "verify"               , required_argument, 0, 0   }, "verify a result, provide an accepted range in the form <lower bound>:<upper bound>, like \"--verify 1:1\" if the deck should win all the time" }
                , { { "verbose"              , no_argument      , 0, 'v' }, "verbose output" }
                , { { "seed"                 , optional_argument, 0, 0   }, "set the seed, takes an optional argument. if none given use seed based on time." }
                , { { "color"                , optional_argument, 0, 0   }, "color the output, currently only ANSI colors supported" }
                , { { "help"                 , no_argument      , 0, 'h' }, "print help" }
                , { { "surge"                , no_argument      , 0, 's' }, "first deck will surge" }
                , { { "raid-id"              , required_argument, 0, 'r' }, "raid id" }
                , { { "quest-id"             , required_argument, 0, 'Q' }, "quest id" }
                , { { "mission-id"           , required_argument, 0, 'm' }, "mission id" }
                };


            static size_t const numberOfOptions = sizeof(options)/sizeof(CommentedOption);

            void printUsage();

            /**
            * Default values.
            */
            #define DEFAULT_NUMBER_OF_ITERATIONS 1000

            class DeckArgument {
                public:
                    enum DeckArgumentType {
                            HASH,
                            MISSION_ID,
                            RAID_ID,
                            QUEST_IDy
                    };
                private:
                    DeckArgumentType type;

                    struct DeckArgumentHashData {
                        std::string hash;
                        bool ordered;

                        DeckArgumentHashData();
                    };
                    DeckArgumentHashData hashData;
                    unsigned int missionId;
                    unsigned int raidId;
                    unsigned int questId;
                public:
                    DeckArgument();

                    DeckArgumentType getType() const;
                    std::string getHash() const throw (InvalidState);
                    int getMissionId() const throw (InvalidState);
                    int getQuestId() const throw (InvalidState);
                    int getRaidId() const throw (InvalidState);
                    bool isOrdered() const throw (InvalidState);

                    void setHash(std::string const & hash);
                    void setMission(int const & missionId);
                    void setQuest(int const & questId);
                    void setRaid(int const & raidId);
                    void setOrdered(bool const & ordered);
            };

            struct AchievementOptions {
                private:
                    bool doAchievementCheck;
                    unsigned int achievementIndex;
                public:
                    AchievementOptions();

                    void enableCheck(unsigned int const & achievementIndex);
                    void disableCheck();
                    operator int() const;
            };

            struct CliOptions {
                DeckArgument attackDeck;
                DeckArgument defenseDeck;

                unsigned int numberOfIterations;
                unsigned int verbosity;
                AchievementOptions achievementOptions;
                VerifyOptions verifyOptions;
                unsigned int seed;
                Logger::ColorMode colorMode;
                bool printHelpAndExit;
                bool surge;

                CliOptions();
            };
        } // namespace EvaluateDecks::CLI
    } // namespace EvaluateDecks

#endif
