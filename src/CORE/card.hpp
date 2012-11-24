#ifndef ITERATEDECKS_CORE_CARD_HPP
    #define ITERATEDECKS_CORE_CARD_HPP

    #include "simpleTypes.hpp"
    #include "constants.hpp"

    namespace IterateDecks {
        namespace Core {

            class Card {
                private:
                    UINT Id;
                    char Name[CARD_NAME_MAX_LENGTH];
                    char Picture[FILENAME_MAX_LENGTH];
                    UCHAR Type;
                    UCHAR Wait;
                    UINT Set;
                    UCHAR Faction;
                    UCHAR Attack;
                    UCHAR Health;
                    Rarity rarity;
                    EFFECT_ARGUMENT Effects[CARD_ABILITIES_MAX];
                    UCHAR TargetCounts[CARD_ABILITIES_MAX];
                    UCHAR TargetFactions[CARD_ABILITIES_MAX]; // reserved negative values for faction
                    UCHAR AbilityEvent[CARD_ABILITIES_MAX];
                #define RESERVE_ABILITIES_COUNT	3
                    std::vector<UCHAR> AbilitiesOrdered;
                protected:
                    void CopyName(const char *name);
                    void CopyPic(const char *pic);
                public:
                    Card();
                    Card(const UINT id, const char* name, const char* pic, Rarity const rarity, const UCHAR type, const UCHAR faction, const UCHAR attack, const UCHAR health, const UCHAR wait, const UINT set);
                    Card(const Card &card);
                    Card& operator=(const Card &card);
                    void AddAbility(const UCHAR id, const EFFECT_ARGUMENT effect, const UCHAR targetcount, const UCHAR targetfaction, const UCHAR skillevent = EVENT_EMPTY);
                    void AddAbility(const UCHAR id, const UCHAR targetcount, const UCHAR targetfaction);
                    void AddAbility(const UCHAR id, const EFFECT_ARGUMENT effect);
                    void AddAbility(const UCHAR id);
                    void PrintAbilities();
                    void Destroy();
                    ~Card();
                    const bool IsCard() const;
                    const UINT GetId() const;
                    const char* GetID16(UINT &ID16Storage, bool bLowerCase = false) const;
                    const unsigned short GetID64() const;
                    const char* GetID64(UINT &ID64Storage) const;
                    const UCHAR GetAttack() const;
                    const UCHAR GetHealth() const;
                    const UCHAR GetWait() const;
                    const UCHAR GetType() const;
                    const UCHAR GetSet() const;
                    const UCHAR GetRarity() const;
                    const UCHAR GetFaction() const;
                    const EFFECT_ARGUMENT GetAbility(const UCHAR id) const;
                    const UCHAR GetAbilitiesCount() const;
                    const UCHAR GetAbilityInOrder(const UCHAR order) const;
                    const UCHAR GetTargetCount(const UCHAR id) const;
                    const UCHAR GetTargetFaction(const UCHAR id) const;
                    const UCHAR GetAbilityEvent(const UCHAR id) const;
                    const char *GetName() const;
                    const char *GetPicture() const;
            };

        }
    }

#endif
