#ifndef CLI_OPTIONS_H_
    #define CLI_OPTIONS_H_

    #include "verify.hpp"
    #include "../EXE/exceptions.hpp"

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
                    QUEST_ID   
            };
        private:
            DeckArgumentType type;

            struct DeckArgumentHashData {
                std::string hash;
                bool ordered;
            };
            DeckArgumentHashData hashData;
            unsigned int missionId;
            unsigned int raidId;
            unsigned int questId;
        public:
            DeckArgument();

            DeckArgumentType getType() const;
            std::string getHash() const throw (InvalidState);
            bool isOrdered() const throw (InvalidState);

            void setHash(std::string const & hash);
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

        CliOptions(); 
    };

#endif
