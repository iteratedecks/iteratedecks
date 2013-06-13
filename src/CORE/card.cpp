#include "card.hpp"
#include "assert.hpp"

// TODO rewrite old printf to c++ style
#include <cstdio>
#include <cstring>

namespace IterateDecks {
    namespace Core {

        char const UNDEFINED_NAME[] = "UNDEFINED";

        unsigned short ID2BASE64(const UINT Id)
        {
            assertX(Id < 0xFFF);
            #define EncodeBase64(x) (x < 26) ? (x + 'A') : ((x < 52) ? (x + 'a' - 26) : ((x < 62) ? (x + '0' - 52) : ((x == 62) ? ('+') : ('/'))))
            // please keep in mind that any integer type has swapped hi and lo bytes
            // i have swapped them here so we will have correct 2 byte string in const char* GetID64 function
            return ((EncodeBase64(((Id >> 6) & 63)))/* << 8*/) + ((EncodeBase64((Id & 63))) << 8); // so many baneli... parenthesis!
            #undef EncodeBase64
        }

        void Card::CopyName(const char *name)
        {
            if (name) {
                size_t len = strlen(name);
                memcpy(Name,name,len);
                Name[len] = 0;
            } else {
                memcpy(Name,UNDEFINED_NAME,sizeof(UNDEFINED_NAME)+1);
            }
        }

        void Card::CopyPic(const char *pic)
        {
            if (pic) {
                size_t len = strlen(pic);
                memcpy(Picture,pic,len);
                Picture[len] = 0;
            } else {
                memcpy(Picture,UNDEFINED_NAME,sizeof(UNDEFINED_NAME)+1);
            }
        }

        Card::Card()
        : Id(0)
        {
            memset(Name,0,CARD_NAME_MAX_LENGTH);
            memset(Effects,0,CARD_ABILITIES_MAX * sizeof(EFFECT_ARGUMENT));
            memset(TargetCounts,0,CARD_ABILITIES_MAX);
            memset(TargetFactions,0,CARD_ABILITIES_MAX);
            memset(AbilityEvent,0,CARD_ABILITIES_MAX);
            //AbilitiesOrdered.reserve(RESERVE_ABILITIES_COUNT);
        }
        Card::Card(UINT const id
                  ,char const * name
                  ,const char* pic
                  ,Rarity const rarity
                  ,CardType const type
                  ,Faction const faction, const UCHAR attack, const UCHAR health, const UCHAR wait, const UINT set)
        : Id(id)
        , type(type)
        , faction(faction)
        , rarity(rarity)
        {
            assertX(id != 0);
            assertX(type != TYPE_NONE);
            //UINT temp = ID2BASE64(4000);
            //printf("%s -> %d\n",(char*)&temp,BASE64ID(temp));
            //BASE642ID(temp);
            CopyName(name);
            CopyPic(pic);
            Attack = attack;
            Health = health;
            Wait = wait;
            Set = set;
            memset(Effects,0,CARD_ABILITIES_MAX*sizeof(EFFECT_ARGUMENT));
            memset(TargetCounts,0,CARD_ABILITIES_MAX);
            memset(TargetFactions,0,CARD_ABILITIES_MAX);
            memset(AbilityEvent,0,CARD_ABILITIES_MAX);
            AbilitiesOrdered.reserve(RESERVE_ABILITIES_COUNT);
        }
        Card::Card(Card const & card)
        : type(card.type)
        , faction(card.faction)
        , rarity(card.rarity)
        , abilities(abilities)
        {
            Id = card.Id;
            memcpy(Name,card.Name,CARD_NAME_MAX_LENGTH);
            memcpy(Picture,card.Picture,FILENAME_MAX_LENGTH);
            Attack = card.Attack;
            Health = card.Health;
            Wait = card.Wait;
            Set = card.Set;
            memcpy(Effects,card.Effects,CARD_ABILITIES_MAX*sizeof(EFFECT_ARGUMENT));
            memcpy(TargetCounts,card.TargetCounts,CARD_ABILITIES_MAX);
            memcpy(TargetFactions,card.TargetFactions,CARD_ABILITIES_MAX);
            memcpy(AbilityEvent,card.AbilityEvent,CARD_ABILITIES_MAX);
            AbilitiesOrdered.reserve(RESERVE_ABILITIES_COUNT);
            AbilitiesOrdered = card.AbilitiesOrdered;            
        }
        Card& Card::operator=(Card const &card)
        {
            Id = card.Id;
            memcpy(Name,card.Name,CARD_NAME_MAX_LENGTH);
            memcpy(Picture,card.Picture,FILENAME_MAX_LENGTH);
            this->type = card.type;
            this->faction = card.faction;
            Attack = card.Attack;
            Health = card.Health;
            Wait = card.Wait;
            this->rarity = card.rarity;
            Set = card.Set;
            memcpy(Effects,card.Effects,CARD_ABILITIES_MAX*sizeof(EFFECT_ARGUMENT));
            memcpy(TargetCounts,card.TargetCounts,CARD_ABILITIES_MAX);
            memcpy(TargetFactions,card.TargetFactions,CARD_ABILITIES_MAX);
            memcpy(AbilityEvent,card.AbilityEvent,CARD_ABILITIES_MAX);
            AbilitiesOrdered.reserve(RESERVE_ABILITIES_COUNT);
            AbilitiesOrdered = card.AbilitiesOrdered;
            this->abilities = card.abilities;
            return *this;
        }
        
        void
        Card::AddAbility(AbilityEnum const abilityType
                        ,AbilityArgument const effect
                        ,TargetCount const targetCount
                        ,Faction const targetFaction
                        ,EventCondition const condition)
        {
            Effects[abilityType] = effect;
            TargetCounts[abilityType] = targetCount;
            TargetFactions[abilityType] = targetFaction;
            AbilityEvent[abilityType] = condition;
            AbilitiesOrdered.push_back(abilityType);
            this->abilities.push_back(
                Ability::createAbility(abilityType, targetCount, targetFaction, effect, condition)
            );
        }
        void Card::AddAbility(const AbilityEnum id, const TargetsCount targetcount, const Faction targetfaction)
        {
            this->AddAbility(id, ABILITY_ENABLED, targetcount, targetfaction, EVENT_EMPTY);
        }
        void Card::AddAbility(const AbilityEnum id, const EFFECT_ARGUMENT effect)
        {
            this->AddAbility(id, effect, TARGETSCOUNT_ONE, FACTION_NONE, EVENT_EMPTY);
        }
        void Card::AddAbility(const AbilityEnum id)
        {
            this->AddAbility(id, ABILITY_ENABLED);
        }
        
        void Card::PrintAbilities()
        {
            for (UCHAR i=0;i<CARD_ABILITIES_MAX;i++)
                if (Effects[i] > 0)
                    printf("%d ",i);
            printf("\n");
        }
        void Card::Destroy() {	Id = 0;	}
        Card::~Card()	{ Destroy(); }
        const bool Card::IsCard() const { return (Id != 0); }
        const UINT Card::GetId() const { return Id; }
        const char* Card::GetID16(UINT &ID16Storage, bool bLowerCase) const
        {
            UCHAR c = Id & 0xF;
            UCHAR baseA = (bLowerCase) ? ('a' - 10) : ('A' - 10); // I thought I told I don't like this style ;)
            char *ptr = (char *)&ID16Storage;
            c = (c < 10) ? (c + '0') : (c + baseA);
            ptr[2] = c;
            c = (Id >> 4) & 0xF;
            c = (c < 10) ? (c + '0') : (c + baseA);
            ptr[1] = c;
            c = (Id >> 8) & 0xF;
            c = (c < 10) ? (c + '0') : (c + baseA);
            ptr[0] = c;
            ptr[3] = 0;
            return (const char*)ptr;
        }
        //const unsigned short Card::GetID64() const
        //{
        //    return ID2BASE64(Id);
        //}
        //const char* Card::GetID64(UINT &ID64Storage) const
        //{
        //    //ID64Storage = 0; // is it nessesary?
        //    ID64Storage = GetID64();
        //    return (char *)&ID64Storage;
        //}
        const UCHAR Card::GetAttack() const	{	return Attack;	}
        const UCHAR Card::GetHealth() const	{	return Health;	}
        const UCHAR Card::GetWait() const		{	return Wait;	}
        CardType Card::GetType() const		{	return this->type;	}
        const UCHAR Card::GetSet() const		{	return Set;		}
        Rarity Card::GetRarity() const { return this->rarity; }
        Faction Card::GetFaction() const { return this->faction; }
        const EFFECT_ARGUMENT Card::GetAbility(const UCHAR id) const { return Effects[id]; }
        const UCHAR Card::GetAbilitiesCount() const { return (UCHAR)AbilitiesOrdered.size(); }
        const UCHAR Card::GetAbilityInOrder(const UCHAR order) const
        {
            //assertX(AbilitiesOrdered.size() > order); DISABLED FOR THE PURPOSE OF QUEST EFFECT TIME SURGE
            if (AbilitiesOrdered.size() <= order)
                return 0;
            else
                return AbilitiesOrdered[order];
        }
        const TargetsCount Card::GetTargetCount(const UCHAR id) const { return TargetCounts[id]; }
        const Faction Card::GetTargetFaction(const UCHAR id) const { return TargetFactions[id]; }
        const UCHAR Card::GetAbilityEvent(const UCHAR id) const { return AbilityEvent[id]; }
        const char * Card::GetName() const { return Name; }
        const char * Card::GetPicture() const { return Picture; }

        Ability::Ptr
        Card::getAbility(size_t index)
        {
            if(index < this->abilities.size()) {
                return this->abilities[index];
            } else {
                std::stringstream ssMessage;
                ssMessage << "Index out of bounds: ";
                ssMessage << index << " !< " << this->abilities.size();
                throw LogicError(ssMessage.str());
            }
        }
        Ability::ConstPtr
        Card::getAbility(size_t index) const
        {
            if(index < this->abilities.size()) {
                return this->abilities[index];
            } else {
                std::stringstream ssMessage;
                ssMessage << "Index out of bounds: ";
                ssMessage << index << " !< " << this->abilities.size();
                throw LogicError(ssMessage.str());
            }
        }

    }
}
