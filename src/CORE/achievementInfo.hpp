#ifndef ITERATEDECKS_CORE_ACHIEVEMENTINFO_HPP
    #define ITERATEDECKS_CORE_ACHIEVEMENTINFO_HPP

    #include <string>
    #include <vector>
    #include "simpleTypes.hpp"

    namespace IterateDecks {
        namespace Core {

            enum AchievementRequirementCompare {
                UNDEFINED = 0,
                EQUAL = 1,
                LESS = 2,
                LESSEQUAL = 3,
                GREATER = 4,
                GREATEREQUAL = 5,
            };

            struct AchievementRequirement {
                public:
                    short NumTurns;
                    //
                    short UnitID;
                    short UnitRace;
                    short UnitRarity;
                    short NumPlayed;
                    //
                    short UnitType;
                    short NumKilled;
                    //
                    short SkillID;
                    short NumUsed;
                    short NumKilledWith;
                    bool Status; // ???
                    //
                    short Damage;
                    short ComTotal;
                    AchievementRequirementCompare Compare;

                    AchievementRequirement();
            };

            class AchievementInfo {
                struct AchievementType {
                        char EnemyID;
                        short MissionID;
                        bool Winner;
                        UCHAR Compare;
                        AchievementType();
                        const bool IsValid() const;
                        const bool CheckMissionID(short missionID) const;
                };
                private:
                    UINT Id;
                    std::string Name;
                    std::string Description;
                    UINT Number;
                    AchievementType Type;
                public:
                    std::vector<AchievementRequirement> Reqs;
                public:
                    AchievementInfo();
                    AchievementInfo(const UINT id, const char *name = 0, const char *desc = 0, const UINT number = 0);
                    void SetType(const char *enemyID, UINT missionID, bool winner, UCHAR compare);
                    void AddRequirement(AchievementRequirement const & r);
                    const bool IsValid();
                    const char *GetName() const;
                    const char *GetDescription() const;
                    const UINT GetID() const;
                    const bool CheckMissionID(UINT const MissionID) const;
            };

        }
    }
#endif
