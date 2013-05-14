#include "achievementOptions.hpp"

namespace IterateDecks {
    namespace Core {
        
        AchievementOptions::AchievementOptions()
        : doAchievementCheck(false)
        , achievementId(0)
        {
        }

        void AchievementOptions::enableCheck(unsigned int const & achievementId)
        {
            this->doAchievementCheck = true;
            this->achievementId = achievementId;
        }

        void AchievementOptions::disableCheck()
        {
            this->doAchievementCheck = false;
            this->achievementId = 0;
        }

        AchievementOptions::operator int() const
        {
            if(this->doAchievementCheck) {
                return this->achievementId;
            } else {
                return -1;
            }
        }
        
    }
}
