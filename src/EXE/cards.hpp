#ifndef CARDS_HPP_
    #define CARDS_HPP_

    #include <set>
    #include <map>
    #include <vector>
    #include <utility>
    #include "deck.hpp"

    #define TAG_ANY			-100500
    #define TAG_BATCHEVAL	100500
    #define TAG_CUSTOM		0
    #define TAG_SOMERAID	-1000
    #define TAG_SOMEMISSION	1000

    #define CARD_MAX_ID				4000 // sizes of storage arrays
    #define MISSION_MAX_ID			400
    #define ACHIEVEMENT_MAX_COUNT	300
    #define RAID_MAX_ID				30
    #define BATTLEGROUND_MAX_ID		20
    #define STEP_MAX_ID				40

    typedef std::map<std::string, UINT> MSUINT;
    typedef std::pair<std::string, UINT> PAIRMSUINT;
    typedef std::vector<std::string> VSTRINGS;
    typedef std::map<std::string, UCHAR> MSKILLS;
    typedef std::pair<std::string, UCHAR> PAIRMSKILLS;
    typedef std::set <UINT> SCID;
    typedef std::map<UINT, UINT> MUUINT;
    typedef std::pair<UINT, UINT> PAIRMUUINT;

    struct DeckIndex;
    typedef std::map<DeckIndex, VSTRINGS> MDECKS;
    typedef std::pair<DeckIndex, VSTRINGS> PAIRMDECKS;

    struct AchievementRequirement;

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
            void AddRequirement(AchievementRequirement &r);
            const bool IsValid();
            const char *GetName() const;
            const char *GetDescription() const;
            const UINT GetID() const;
            const bool CheckMissionID(UINT MissionID) const;
    };

    struct CardSet;
    typedef std::map<UINT, CardSet> MSETS;
    typedef std::pair<UINT, CardSet> PAIRMSETS;
    typedef std::vector<UINT> VID;

    class MissionInfo {
        private:
            std::string Name;
            UINT Commander;
            VID Deck;
        public:
            MissionInfo();
            MissionInfo(const UINT commander, const char *name = 0);
            void Add(const UINT cardID);
            bool GetDeck(char *buffer, size_t buffersize);
            const UINT GetCommander() const;
            const char *GetName() const;
            const UINT GetCardCount() const;
            const UINT GetCardID(UINT Index) const;
            ~MissionInfo();
            const bool IsValid() const;
    };

    class CardPool;
    typedef std::vector<CardPool> VCARDPOOL;

    class RaidInfo {
        private:
            std::string Name;
            UINT Commander;
            VID AlwaysInclude;
            VCARDPOOL Pools;
        public:
            RaidInfo();
            RaidInfo(UINT commander, const char *name);
            RaidInfo(RaidInfo const &RI);
            void GetAlways(Card *pCDB, LCARDS &Deck) const;
            void GetPools(Card *pCDB, LCARDS &Deck) const;
            const UINT GetCommander();
            const char *GetName() const;
            void AddAlways(const UINT cardID);
            void AddPool(const CardPool &p);
            bool IsValid();
            ~RaidInfo();
    };

    class BgInfo {
        private:
            UINT Id;
            std::string Name;
            std::string Desc;
            UINT EffectId;
        public:
            BgInfo();
            BgInfo(const UINT id,const char *name,const char *desc,const UINT effectid);
            BgInfo(const BgInfo &BI);
            const char *GetName();
            const UINT GetEffectId();
    };

    class StepInfo {
        private:
            UINT Id;
            UINT BgId;
            UINT Commander;
            VCARDPOOL Pools;
        public:
            StepInfo();
            StepInfo(UINT id, UINT bgid, UINT commander);
            StepInfo(StepInfo const &SI);
            void GetPools(Card *pCDB, LCARDS &Deck) const;
            const UINT GetCommander();
            const UINT GetBGId();
            void AddPool(const CardPool &p);
            bool IsValid();
            ~StepInfo();
    };

    struct SKILL {
        char SkillName[CARD_NAME_MAX_LENGTH];
        bool IsPassive;
        float CardValue;
        SKILL();
    };

    class CardDB {
        private:
            Card CDB[CARD_MAX_ID]; // this can cause stack overflow, but should work fastest
            MissionInfo MDB[MISSION_MAX_ID];
            AchievementInfo ADB[ACHIEVEMENT_MAX_COUNT];
            RaidInfo RDB[RAID_MAX_ID];
            BgInfo BGDB[BATTLEGROUND_MAX_ID];
            StepInfo STDB[STEP_MAX_ID];
            MSUINT MIIndex;
            MSUINT AIIndex;
            MSUINT RIIndex;
            MSUINT Index;
            MDECKS DIndex;
            //char SKILLS[CARD_ABILITIES_MAX][CARD_NAME_MAX_LENGTH];
            MSKILLS SIndex;
            MSKILLS QuestEffectIndex;
        public://protected:
            SKILL Skills[CARD_ABILITIES_MAX];
            MSETS SetIndex;

            CardDB();
        private: // helper functions
            bool SaveIndex(const char *FileName, MDECKS &index);
            UCHAR DetectTypeByID(UINT ID);
            UCHAR RemapFaction(UCHAR XmlID);
            UCHAR RemapRarity(UCHAR XmlID, UCHAR UniqueFlag = 0);
        public:
            const Card *GetPointer() const;
            bool LoadCardXML(const char *FileName, char *returnnewcards = 0, size_t MaxBufferSize = 0);
            static const UCHAR RemapUnitType(const UCHAR unitType);
            bool LoadAchievementXML(const char *FileName);
            bool LoadMissionXML(const char *FileName);
            bool LoadRaidXML(const char *FileName);
            bool LoadQuestXML(const char *FileName);
            const UINT GetQuestEffectId(UINT QuestId);
            bool RateCard(const UINT Id, double &OffenceValue, double &DefenceValue, const UCHAR iFormulaVersion = 0);
            void GenRaidDeck(ActiveDeck &D, UINT RaidID);
            void GenQuestDeck(ActiveDeck &D, UINT QuestID);
            DWORD LoadPointers(Card **Ptr, DWORD MaxCount);
            void Initialize();
            void AddSkill(UCHAR Id, const char *Name);
            void AddQuestEffect(UCHAR Id, const char *Name);
            const char *GetSkill(UCHAR Indx) const;
            UCHAR GetSkillID(const char *Name);
            UCHAR GetSkillIDSlow(const char *Name);
            size_t GetSize();
            bool Insert(Card &c);
            bool CreateDeck(const char *CardsList, ActiveDeck &D);
            char *trim(char *str, char c=' ') const;
            bool SaveCustomDecks(const char *FileName);
            bool SaveMissionDecks(const char *FileName);
            bool ClearDeckIndex();
            int LoadDecks(const char *FileName, bool bRewrite = false);
            int LoadOwnedCards(const char *FileName, MUUINT &OwnedCards);
            int LoadCardList(const char *FileName, SCID &CardPool) const;
            bool InsertDeck(int Tag, const char *List, char *output_id_buffer = 0);
            void GetAchievementList(char *Buffer, DWORD MaxBufferSize);
            const char *GetAchievementDescription(DWORD AchievementIndex);
            const UINT GetAchievementID(DWORD AchievementIndex);
            bool CheckAchievementMission(UINT AchievementIndex, UINT MissionID);
            void Print();
            const Card *CARD(const char *Name);
            const Card *GetCard(std::string Name) const;
            const Card *GetCard(const char *Name) const;
            const Card &GetCard(const UINT Id) const;
            const Card *GetCardSmart(const char *Name) const;
            const bool GetCardListSorted(char *buffer, DWORD size);
            const char* GetCustomDecksList(char *buffer, size_t size, int byTag = TAG_ANY);
            const char* GetMissionDecksList(char *buffer, size_t size, bool bSortById = false);
            const char* GetCustomDeck(const char* DeckName, const int Tag, char *buffer, size_t size);
            const char* GetMissionDeck(const char* DeckName, char *buffer, size_t size);
            ActiveDeck CardDB::GetMissionDeck(const UINT missionId);
            const UINT GetMissionDeckIndex(const char* DeckName);
            const char* GetRaidDecksList(char *buffer, size_t size);
            const char* GetQuestDecksList(char *buffer, size_t size);
            const UINT GetRaidCommanderID(UINT RaidIndex);
            const UINT GetQuestCommanderID(UINT QuestIndex);
            void GenRandomDeckFromCard(VCARDS &Deck,size_t i);
            const UINT GetCustomCount();
            bool GetCustomDeck(UINT Index, ActiveDeck &R);
            bool CheckAchievement(int achievementIndex, const UINT iTurn, ActiveDeck &Atk, ActiveDeck &Def);
            // named decks
            ActiveDeck GetNamedDeck(const char* DeckName, const int Tag);
    };

#endif
