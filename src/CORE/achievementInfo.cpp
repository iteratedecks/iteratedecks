#include "achievementInfo.hpp"

namespace IterateDecks {
    namespace Core {

        AchievementRequirement::AchievementRequirement()
        : NumTurns(-1)
        , UnitID(-1)
        , UnitRace(-1)
        , UnitRarity(-1)
        , NumPlayed(-1)
        , UnitType(-1)
        , NumKilled(-1)
        , SkillID(-1)
        , NumUsed(-1)
        , NumKilledWith(-1)
        , Status()
        , Damage(-1)
        , ComTotal(-1)
        , Compare(UNDEFINED)
        {}


        AchievementInfo::AchievementType::AchievementType()
        : EnemyID()
        {
            MissionID = 0;
            Winner = 0;
            Compare = 0;
        }

        bool const AchievementInfo::AchievementType::IsValid() const
        {
            return (!EnemyID);//(MissionID != 0);
        }

        bool const AchievementInfo::AchievementType::CheckMissionID(short missionID) const
        {
            if (!this->MissionID) {
                // skip if mission ID is undefined
                return true;
            }
            switch(this->Compare) {
                case UNDEFINED:
                    return true;
                case EQUAL:
                    return missionID == this->MissionID;
                case GREATEREQUAL:
                    return missionID >= this->MissionID;
                case GREATER:
                    return missionID > this->MissionID;
                case LESSEQUAL:
                    return missionID <= this->MissionID;
                case LESS:
                    return missionID < this->MissionID;
                default:
                    return true;
            }
        }

        AchievementInfo::AchievementInfo()
        : Id(0)
        , Number()
        {}

        AchievementInfo::AchievementInfo(UINT const id, char const * const name, char const * const desc, UINT const number)
        : Id(id)
        , Number(number)
        {
            if (name != NULL) {
                Name = std::string(name);
            }
            if (desc != NULL) {
                Description = std::string(desc);
            }
            // Reqs.clear(); // thats pointless: at this point the vector is guaranted to be empty
        }
        void AchievementInfo::SetType(char const * const enemyID, UINT missionID, bool winner, UCHAR compare)
        {
            if (enemyID != NULL && enemyID[0]) {
                // so this will set the first letter of enemyID... this is a bit strange
                this->Type.EnemyID = enemyID[0];
            } else {
                this->Type.EnemyID = 0;
            }
            this->Type.MissionID = missionID;
            this->Type.Winner = winner;
            this->Type.Compare = compare;
        }

        void AchievementInfo::AddRequirement(AchievementRequirement const & r)
        {
            this->Reqs.push_back(r);
        }

        const bool AchievementInfo::IsValid()
        {
            return    (this->Id != 0)
                   && (Type.IsValid())
                   && (Reqs.size() > 0)
                   && (this->Number <= 1);
        }
        const char *AchievementInfo::GetName() const
        {
            return this->Name.c_str();
        }
        const char *AchievementInfo::GetDescription() const
        {
            return this->Description.c_str();
        }
        const UINT AchievementInfo::GetID() const
        {
            return this->Id;
        }
        const bool AchievementInfo::CheckMissionID(UINT const MissionID) const
        {
            return this->Type.CheckMissionID(MissionID);
        }
    }
}
