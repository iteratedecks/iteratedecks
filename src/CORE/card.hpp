#ifndef ITERATEDECKS_CORE_CARD_HPP
    #define ITERATEDECKS_CORE_CARD_HPP

    #include "simpleTypes.hpp"
    #include "constants.hpp"
    #include "ability.hpp"

    namespace IterateDecks {
        namespace Core {

            class Card {
                private:
                    UINT Id;
                    char Name[CARD_NAME_MAX_LENGTH];
                    char Picture[FILENAME_MAX_LENGTH];
                    CardType type;
                    UCHAR Wait;
                    UINT Set;
                    Faction faction;
                    UCHAR Attack;
                    UCHAR Health;
                    Rarity rarity;
                    std::vector<Ability::Ptr> abilities;
                    EFFECT_ARGUMENT Effects[CARD_ABILITIES_MAX];
                    TargetsCount TargetCounts[CARD_ABILITIES_MAX];
                    Faction TargetFactions[CARD_ABILITIES_MAX]; // reserved negative values for faction
                    EVENT_CONDITION AbilityEvent[CARD_ABILITIES_MAX];
                #define RESERVE_ABILITIES_COUNT	3
                    std::vector<UCHAR> AbilitiesOrdered;
                protected:
                    void CopyName(const char *name);
                    void CopyPic(const char *pic);
                public:
                    Card();
                    Card(const UINT id, const char* name, const char* pic, Rarity const rarity, CardType const type, Faction const faction, const UCHAR attack, const UCHAR health, const UCHAR wait, const UINT set);
                    Card(const Card &card);
                    Card& operator=(const Card &card);
                    void AddAbility(AbilityEnum const id, AbilityArgument const effect, TargetCount const targetCount, Faction const targetFaction, EventCondition const skillEvent = EVENT_EMPTY);
                    void AddAbility(AbilityEnum const id, TargetCount const targetCount, Faction const targetFaction);
                    void AddAbility(AbilityEnum const id, AbilityArgument const effect);
                    void AddAbility(AbilityEnum const id);
                    void PrintAbilities();
                    void Destroy();
                    ~Card();
                    const bool IsCard() const;
                    const UINT GetId() const;
                    const char* GetID16(UINT &ID16Storage, bool bLowerCase = false) const;
                    //const unsigned short GetID64() const;
                    //const char* GetID64(UINT &ID64Storage) const;
                    const UCHAR GetAttack() const;
                    const UCHAR GetHealth() const;
                    const UCHAR GetWait() const;
                    CardType GetType() const;
                    const UCHAR GetSet() const;
                    Rarity GetRarity() const;
                    Faction GetFaction() const;
                    const EFFECT_ARGUMENT GetAbility(const UCHAR id) const;
                    const UCHAR GetAbilitiesCount() const;
                    const UCHAR GetAbilityInOrder(const UCHAR order) const;
                    const TargetsCount GetTargetCount(const UCHAR id) const;
                    const Faction GetTargetFaction(const UCHAR id) const;
                    const UCHAR GetAbilityEvent(const UCHAR id) const;
                    const char *GetName() const;
                    const char *GetPicture() const;
                    
                    Ability::Ptr getAbility(size_t index);
                    Ability::ConstPtr getAbility(size_t index) const;        
            };

        }
    }

#endif
