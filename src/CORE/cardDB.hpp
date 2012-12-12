#ifndef ITERATEDECKS_CORE_CARDDB_HPP
    #define ITERATEDECKS_CORE_CARDDB_HPP

    #include "card.hpp"
    #include "missionInfo.hpp"
    #include "raidInfo.hpp"
    #include "bgInfo.hpp"
    #include "achievementInfo.hpp"
    #include "stepInfo.hpp"
    #include "deckIndex.hpp"
    #include "skill.hpp"
    #include "cardSet.hpp"
    #include "simpleTypes.hpp"

    #include "activeDeck.forward.hpp"

    // FIXME: Most ActiveDeck references should probably be rather some InactiveDeck

    namespace IterateDecks {
        namespace Core {

            class CardDB {
                private:
                    Card CDB[CARD_MAX_ID]; // NR?: this can cause stack overflow, but should work fastest
                    //^ P: I doubt that a pointer to the stack is significantly faster than a pointer to the heap.
                    //^ P: Also this gives us a compile time dependency on card (not a big deal)

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
                    MapBattleGroundEffects QuestEffectIndex;
                public://protected:
                    Skill Skills[CARD_ABILITIES_MAX];
                    MSETS SetIndex;

                    CardDB();
                private: // helper functions
                    bool SaveIndex(const char *FileName, MDECKS &index);
                    static CardType DetectTypeByID(UINT ID);
                    static Faction RemapFaction(UCHAR XmlID);
                    static Rarity RemapRarity(UCHAR XmlID, UCHAR UniqueFlag = 0);
                public:
                    const Card *GetPointer() const;
                    bool LoadCardXML(const char *FileName, char *returnnewcards = 0, size_t MaxBufferSize = 0);
                    static const UCHAR RemapUnitType(const UCHAR unitType);
                    bool LoadAchievementXML(const char *FileName);
                    bool LoadMissionXML(const char *FileName);
                    bool LoadRaidXML(const char *FileName);
                    bool LoadQuestXML(const char *FileName);
                    const BattleGroundEffect GetQuestEffectId(UINT QuestId);
                    bool RateCard(const UINT Id, double &OffenceValue, double &DefenceValue, const UCHAR iFormulaVersion = 0);
                    void GenRaidDeck(ActiveDeck &D, UINT RaidID);
                    void GenQuestDeck(ActiveDeck &D, UINT QuestID);
                    DWORD LoadPointers(Card **Ptr, DWORD MaxCount);
                    void Initialize();
                    void AddSkill(UCHAR Id, const char *Name);
                    void AddQuestEffect(BattleGroundEffect effect, const char *Name);
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
                    bool InsertDeck(Tag tag, const char *List, char *output_id_buffer = 0);
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
                    const char* GetCustomDecksList(char *buffer, size_t size, Tag byTag = TAG_ANY);
                    const char* GetMissionDecksList(char *buffer, size_t size, bool bSortById = false);
                    const char* GetCustomDeck(const char* DeckName, Tag const tag, char *buffer, size_t size);
                    const char* GetMissionDeck(const char* DeckName, char *buffer, size_t size);
                    ActiveDeck GetMissionDeck(const UINT missionId) const;
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
                    ActiveDeck GetNamedDeck(const char* DeckName, Tag tag);

                    RaidInfo const & getRaidInfo(unsigned int const raidId) const;
                    StepInfo const & getQuestInfo(unsigned int const questId) const;
            };
        }
    }
#endif
