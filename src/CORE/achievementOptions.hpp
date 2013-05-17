#ifndef ITERATEDECKS_CORE_ACHIEVEMENTOPTIONS_HPP
    #define ITERATEDECKS_CORE_ACHIEVEMENTOPTIONS_HPP

    namespace IterateDecks {
        namespace Core {

            struct AchievementOptions {
                private:
                    bool doAchievementCheck;
                    unsigned int achievementId;
                public:
                    AchievementOptions();

                    void enableCheck(unsigned int const & achievementId);
                    void disableCheck();
                    bool isEnabled() const;
                    operator int() const;
            };

        }
    }
#endif
