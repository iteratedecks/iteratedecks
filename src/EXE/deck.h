// Warning: Strange charset
// *****************************************
// EvaluateDecks
// Tyrant card game simulator
//
// My kongregate account:
// http://www.kongregate.com/accounts/NETRAT
// 
// Project pages:
// http://code.google.com/p/evaluatedecks
// http://www.kongregate.com/forums/65-tyrant/topics/195043-yet-another-battlesim-evaluate-decks
// *****************************************
//
// this module contains all evaluation related stuff - card and deck classes and their interactions

#if defined(__linux__) || defined(__MINGW32__)
// for rand
#include <stdlib.h>
#endif

#include "compat.h"

#include <vector>
#include <list>
#include <set>
#include "ctype.h"
#include <stdexcept>
#include "exceptions.hpp"
#include <iomanip>

#define DEFAULT_DECK_SIZE		10
#define DEFAULT_DECK_RESERVE_SIZE	15 // up to 20? kinda deck max size for structures
#define DEFAULT_HAND_SIZE		3


#include "results.h"
#include "log.h"
#include "Logger.hpp"

#define FANCY_STATS_COUNT		5

#define CARD_MAX_ID				4000 // sizes of storage arrays
#define MISSION_MAX_ID			400
#define ACHIEVEMENT_MAX_COUNT	300
#define RAID_MAX_ID				30
#define BATTLEGROUND_MAX_ID		20
#define STEP_MAX_ID				40

#define RARITY_COMMON			0
#define RARITY_UNCOMMON			1
#define RARITY_RARE				2
#define RARITY_UNIQUE			3
#define RARITY_LEGENDARY		4
#define RARITY_STORYCOMMANDER	10

#define FACTION_NONE			0
#define FACTION_IMPERIAL		1
#define FACTION_RAIDER			2
#define FACTION_BLOODTHIRSTY	3
#define FACTION_XENO			4
#define FACTION_RIGHTEOUS		5


//#define TARGETSCOUNT_NONE		0				
#define TARGETSCOUNT_ONE		0				
#define TARGETSCOUNT_ALL		10	



#define TYPE_NONE				0
#define TYPE_COMMANDER			1
#define TYPE_ASSAULT			2
#define TYPE_STRUCTURE			3
#define TYPE_ACTION				4

#define ABILITY_ENABLED			1  // just a helper, no use

#define SPECIAL_ATTACK			0

#define ACTIVATION_CHAOS		11
#define ACTIVATION_CLEANSE		12
#define ACTIVATION_ENFEEBLE		13
#define ACTIVATION_FREEZE		14
#define ACTIVATION_HEAL			15
#define ACTIVATION_INFUSE		16	// this currently works only for Commander	
#define ACTIVATION_JAM			17
#define ACTIVATION_MIMIC		18
#define ACTIVATION_PROTECT		19
#define ACTIVATION_RALLY		20
#define ACTIVATION_RECHARGE		21
#define ACTIVATION_REPAIR		22
#define ACTIVATION_RUSH			23
#define ACTIVATION_SHOCK		24
#define ACTIVATION_SIEGE		25
#define ACTIVATION_SPLIT		26
#define ACTIVATION_STRIKE		27
#define ACTIVATION_SUMMON       28 // i think 30 is still free... these numbers do not have any other meaning? why not use an enum?
#define ACTIVATION_SUPPLY		29
#define ACTIVATION_WEAKEN		30

#define DEFENSIVE_ARMORED		31
#define DEFENSIVE_COUNTER		32
#define DEFENSIVE_EVADE			33
#define DEFENSIVE_FLYING		34
#define DEFENSIVE_INTERCEPT		35
#define DEFENSIVE_PAYBACK		36
#define DEFENSIVE_REFRESH		37
#define DEFENSIVE_REGENERATE	38
#define DEFENSIVE_TRIBUTE		39
#define DEFENSIVE_WALL			40

#define COMBAT_ANTIAIR			41
#define COMBAT_BURST			42
#define COMBAT_FEAR				43
#define COMBAT_FLURRY			44
#define COMBAT_PIERCE			45
#define COMBAT_SWIPE			46
#define COMBAT_VALOR			47

#define DMGDEPENDANT_BERSERK	51
#define DMGDEPENDANT_CRUSH		52
#define DMGDEPENDANT_DISEASE	53
#define DMGDEPENDANT_IMMOBILIZE	54
#define DMGDEPENDANT_LEECH		55
#define DMGDEPENDANT_POISON		56
#define DMGDEPENDANT_SIPHON		57 

#define SPECIAL_BACKFIRE		61  // Destroyed - When this is destroyed, deal damage to own Commander.

// SKILLS THAT ARE NOT DEFINED AND NOT WORKING:
#define SPECIAL_FUSION			62	// in this sim only works for ACTIVATION skills of STRUCTURES
#define SPECIAL_AUGMENT			63
#define SPECIAL_MIST			64 // this skill doesn't change anything in autoplay // max number refers to CARD_ABILITIES_MAX
#define SPECIAL_BLIZZARD		65 // this skill doesn't change anything in autoplay // max number refers to CARD_ABILITIES_MAX

#define QEFFECT_TIME_SURGE		1
#define QEFFECT_COPYCAT			2
#define QEFFECT_QUICKSILVER		3
#define QEFFECT_DECAY			4
#define QEFFECT_HIGH_SKIES		5
#define QEFFECT_IMPENETRABLE	6
#define QEFFECT_INVIGORATE		7
#define QEFFECT_CLONE_PROJECT	8

#define UNDEFINED_NAME			"UNDEFINED"

using namespace std;

bool bConsoleOutput = false; // this is global just for convinience, should be DEFINE instead, to cleanup the code
int AchievementIndex = -1; // index, not id

#include "deck.hpp"

#define PROC50	Proc()
const bool PROC50
{
	return (rand()%100 < 50);
}

const unsigned short ID2BASE64(const UINT Id)
{
	_ASSERT(Id < 0xFFF);
#define EncodeBase64(x) (x < 26) ? (x + 'A') : ((x < 52) ? (x + 'a' - 26) : ((x < 62) ? (x + '0' - 52) : ((x == 62) ? ('+') : ('/'))))	
	// please keep in mind that any integer type has swapped hi and lo bytes
	// i have swapped them here so we will have correct 2 byte string in const char* GetID64 function
	return ((EncodeBase64(((Id >> 6) & 63)))/* << 8*/) + ((EncodeBase64((Id & 63))) << 8); // so many baneli... parenthesis!
}
#define BASE64ID	BASE642ID // alias
const UINT BASE642ID(const unsigned short base64)
{
#define DecodeBase64(x) (((x >= 'A') && (x <= 'Z')) ? (x - 'A') : (((x >= 'a') && (x <= 'z')) ? (x - 'a' + 26) : (((x >= '0') && (x <= '9')) ? (x - '0' + 52) : ((x == '+') ? (62) : (63)))))
	// same stuff as with ID2BASE64, hi and lo swapped
	return DecodeBase64((base64 & 0xFF)) + DecodeBase64((base64 >> 8)) * 64;
}

class Card
{
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
	UCHAR Rarity;
	EFFECT_ARGUMENT Effects[CARD_ABILITIES_MAX];
	UCHAR TargetCounts[CARD_ABILITIES_MAX];
	UCHAR TargetFactions[CARD_ABILITIES_MAX]; // reserved negative values for faction
	UCHAR AbilityEvent[CARD_ABILITIES_MAX];
#define RESERVE_ABILITIES_COUNT	3
	vector<UCHAR> AbilitiesOrdered;
protected:
	void CopyName(const char *name)
	{
		if (name)
		{
			size_t len = strlen(name);
			memcpy(Name,name,len);
			Name[len] = 0;
		}
		else
			memcpy(Name,UNDEFINED_NAME,sizeof(UNDEFINED_NAME)+1);
	}
	void CopyPic(const char *pic)
	{
		if (pic)
		{
			size_t len = strlen(pic);
			memcpy(Picture,pic,len);
			Picture[len] = 0;
		}
		else
			memcpy(Picture,UNDEFINED_NAME,sizeof(UNDEFINED_NAME)+1);
	}
public:
	Card()
	{
		Id = 0;
		memset(Name,0,CARD_NAME_MAX_LENGTH);
		memset(Effects,0,CARD_ABILITIES_MAX * sizeof(EFFECT_ARGUMENT));
		memset(TargetCounts,0,CARD_ABILITIES_MAX);
		memset(TargetFactions,0,CARD_ABILITIES_MAX);
		memset(AbilityEvent,0,CARD_ABILITIES_MAX);		
		//AbilitiesOrdered.reserve(RESERVE_ABILITIES_COUNT);
	}
	Card(const UINT id, const char* name, const char* pic, const UCHAR rarity, const UCHAR type, const UCHAR faction, const UCHAR attack, const UCHAR health, const UCHAR wait, const UINT set)
	{
		Id = id;
		//UINT temp = ID2BASE64(4000);
		//printf("%s -> %d\n",(char*)&temp,BASE64ID(temp));
		//BASE642ID(temp);
		CopyName(name);
		CopyPic(pic);
		Type = type;
		Faction = faction;
		Attack = attack;
		Health = health;
		Wait = wait;
		Rarity = rarity;
		Set = set;
		memset(Effects,0,CARD_ABILITIES_MAX*sizeof(EFFECT_ARGUMENT));
		memset(TargetCounts,0,CARD_ABILITIES_MAX);
		memset(TargetFactions,0,CARD_ABILITIES_MAX);
		memset(AbilityEvent,0,CARD_ABILITIES_MAX);	
		AbilitiesOrdered.reserve(RESERVE_ABILITIES_COUNT);
	}
	Card(const Card &card)
	{
		Id = card.Id;
		memcpy(Name,card.Name,CARD_NAME_MAX_LENGTH);
		memcpy(Picture,card.Picture,FILENAME_MAX_LENGTH);
		Type = card.Type;
		Faction = card.Faction;
		Attack = card.Attack;
		Health = card.Health;
		Wait = card.Wait;
		Rarity = card.Rarity;
		Set = card.Set;
		memcpy(Effects,card.Effects,CARD_ABILITIES_MAX*sizeof(EFFECT_ARGUMENT));
		memcpy(TargetCounts,card.TargetCounts,CARD_ABILITIES_MAX);
		memcpy(TargetFactions,card.TargetFactions,CARD_ABILITIES_MAX);
		memcpy(AbilityEvent,card.AbilityEvent,CARD_ABILITIES_MAX);	
		AbilitiesOrdered.reserve(RESERVE_ABILITIES_COUNT);
		if (!card.AbilitiesOrdered.empty())
			for (UCHAR i=0;i<card.AbilitiesOrdered.size();i++)
				AbilitiesOrdered.push_back(card.AbilitiesOrdered[i]);
	}
	Card& operator=(const Card &card)
	{
		Id = card.Id;
		memcpy(Name,card.Name,CARD_NAME_MAX_LENGTH);
		memcpy(Picture,card.Picture,FILENAME_MAX_LENGTH);
		Type = card.Type;
		Faction = card.Faction;
		Attack = card.Attack;
		Health = card.Health;
		Wait = card.Wait;
		Rarity = card.Rarity;
		Set = card.Set;
		memcpy(Effects,card.Effects,CARD_ABILITIES_MAX*sizeof(EFFECT_ARGUMENT));
		memcpy(TargetCounts,card.TargetCounts,CARD_ABILITIES_MAX);
		memcpy(TargetFactions,card.TargetFactions,CARD_ABILITIES_MAX);
		memcpy(AbilityEvent,card.AbilityEvent,CARD_ABILITIES_MAX);
		AbilitiesOrdered.reserve(RESERVE_ABILITIES_COUNT);
		if (!card.AbilitiesOrdered.empty())
			for (UCHAR i=0;i<card.AbilitiesOrdered.size();i++)
				AbilitiesOrdered.push_back(card.AbilitiesOrdered[i]);
		return *this;
	}
	void AddAbility(const UCHAR id, const EFFECT_ARGUMENT effect, const UCHAR targetcount, const UCHAR targetfaction, const UCHAR skillevent = EVENT_EMPTY)
	{
		Effects[id] = effect;
		TargetCounts[id] = targetcount;
		TargetFactions[id] = targetfaction;
		AbilityEvent[id] = skillevent;
		AbilitiesOrdered.push_back(id);
	}
	void AddAbility(const UCHAR id, const UCHAR targetcount, const UCHAR targetfaction)
	{
		Effects[id] = ABILITY_ENABLED;
		TargetCounts[id] = targetcount;
		TargetFactions[id] = targetfaction;
		AbilityEvent[id] = EVENT_EMPTY;
		AbilitiesOrdered.push_back(id);
	}
	void AddAbility(const UCHAR id, const EFFECT_ARGUMENT effect)
	{
		Effects[id] = effect;
		AbilityEvent[id] = EVENT_EMPTY;
		AbilitiesOrdered.push_back(id);
	}
	void AddAbility(const UCHAR id)
	{
		Effects[id] = ABILITY_ENABLED;
		AbilityEvent[id] = EVENT_EMPTY;
		AbilitiesOrdered.push_back(id);
	}
	void PrintAbilities()
	{
		for (UCHAR i=0;i<CARD_ABILITIES_MAX;i++)
			if (Effects[i] > 0)
				printf("%d ",i);
		printf("\n");
	}
	void Destroy() {	Id = 0;	}
	~Card()	{ Destroy(); }
	const bool IsCard() const { return (Id != 0); }
	const UINT GetId() const { return Id; }
	const char* GetID16(UINT &ID16Storage, bool bLowerCase = false) const
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
	const unsigned short GetID64() const
	{
		return ID2BASE64(Id);
	}
	const char* GetID64(UINT &ID64Storage) const
	{
		//ID64Storage = 0; // is it nessesary?
		ID64Storage = GetID64();
		return (char *)&ID64Storage;
	}
	const UCHAR GetAttack() const	{	return Attack;	}
	const UCHAR GetHealth() const	{	return Health;	}
	const UCHAR GetWait() const		{	return Wait;	}
	const UCHAR GetType() const		{	return Type;	}
	const UCHAR GetSet() const		{	return Set;		}
	const UCHAR GetRarity() const { return Rarity; }
	const UCHAR GetFaction() const { return Faction; }
	const EFFECT_ARGUMENT GetAbility(const UCHAR id) const { return Effects[id]; }
	const UCHAR GetAbilitiesCount() const { return (UCHAR)AbilitiesOrdered.size(); }
	const UCHAR GetAbilityInOrder(const UCHAR order) const
	{
		//_ASSERT(AbilitiesOrdered.size() > order); DISABLED FOR THE PURPOSE OF QUEST EFFECT TIME SURGE
		if (AbilitiesOrdered.size() <= order)
			return 0;
		else
			return AbilitiesOrdered[order];
	}
	const UCHAR GetTargetCount(const UCHAR id) const { return TargetCounts[id]; }
	const UCHAR GetTargetFaction(const UCHAR id) const { return TargetFactions[id]; }
	const UCHAR GetAbilityEvent(const UCHAR id) const { return AbilityEvent[id]; }
	const char *GetName() const { return Name; }
	const char *GetPicture() const { return Picture; }
};

//class PlayedCard
#if 1
    void PlayedCard::DecWait() { if (Wait) Wait--; }

    void PlayedCard::IncWait() { if (Wait) Wait++; }  // this is only for tournaments

    const char * PlayedCard::GetName() const { return OriginalCard->GetName(); }

    void PlayedCard::PrintDesc()
    {
        if (IsDefined())
        {
            std::cout << this->toString();
        }
    }
    std::string PlayedCard::toString() const
    {
        if (!IsDefined()) {
            return std::string();
        } else {
            std::stringstream ss;
            ss << "[";
            ss << std::setw(3) << this->uniqueId;
            ss << "|";
            switch(this->GetType()){
                case TYPE_ACTION   : ss << 'A'; break;
                case TYPE_ASSAULT  : ss << 'U'; break;
                case TYPE_COMMANDER: ss << 'C'; break;
                case TYPE_STRUCTURE: ss << 'S'; break;
                default: throw logic_error("Unknown card type");
            }
            ss << " \"" << OriginalCard->GetName() << '"';
            if (this->GetType()!=TYPE_ACTION) {
                ss << " ";
                if (this->GetType()==TYPE_ASSAULT) {
                    ss << "A:";
                    ss << (UINT)this->GetAttack();
                    ss << " ";
                }
                ss << "H:";
                ss << (UINT)this->Health;
                if (this->GetType()!=TYPE_COMMANDER) {
                    ss << " D:";
                    ss << (UINT)this->Wait;
                }
                if(this->GetType()==TYPE_ACTION) {
                    if (Effects[ACTIVATION_JAM])
                        ss << " Jammed";
                    if (Effects[ACTIVATION_FREEZE])
                        ss << " Frozen";
                    if (Effects[DMGDEPENDANT_IMMOBILIZE])
                        ss << " Immobilized";
                    if (Effects[DMGDEPENDANT_DISEASE])
                        ss << " Diseased";
                    if (Effects[ACTIVATION_CHAOS])
                        ss << " Chaosed";
                }
            }
            ss << "]";
            return ss.str();
        }
    }
	void PlayedCard::SetCardSkillProcBuffer(UCHAR *_SkillProcBuffer)
	{
		SkillProcBuffer = _SkillProcBuffer;
	}
	const bool PlayedCard::GetQuestSplit() const
	{
		return bQuestSplit;
	}
	void PlayedCard::SetQuestSplit(bool bSplit)
	{
		bQuestSplit = bSplit;
	}
	void PlayedCard::CardSkillProc(UCHAR aid)
	{
		if (SkillProcBuffer) {
			SkillProcBuffer[aid]++;
        }
	}
	const bool PlayedCard::BeginTurn()
	{
		const bool bDoBegin = (Health>0) && (!Effects[ACTIVATION_JAM]) && (!Effects[ACTIVATION_FREEZE]) && (!Wait);
		if (bDoBegin && (!bActivated))
			bActivated = true;
		return bDoBegin;
	}
	void PlayedCard::ProcessPoison(UINT QuestEffectId)
	{
		if (IsAlive() && (Effects[DMGDEPENDANT_POISON]))
			SufferDmg(QuestEffectId,Effects[DMGDEPENDANT_POISON]);
	}
	const UCHAR PlayedCard::GetShield() const
	{
		return Effects[ACTIVATION_PROTECT];
	}
	void PlayedCard::ResetShield()
	{
		Effects[ACTIVATION_PROTECT] = 0;
	}
	void PlayedCard::Refresh(UINT QuestEffectId)
	{
		UCHAR amount = OriginalCard->GetHealth() - Health;
		if (QuestEffectId == QEFFECT_INVIGORATE)
			Attack += amount;
		fsHealed += amount;
		if (SkillProcBuffer && (OriginalCard->GetHealth() != Health))
			SkillProcBuffer[DEFENSIVE_REFRESH]++;
		Health = OriginalCard->GetHealth();
	}
	void PlayedCard::ClearEnfeeble()
	{
		Effects[ACTIVATION_ENFEEBLE] = 0;
	}
	void PlayedCard::RemoveDebuffs()
	{
/*
by Moraku:
Chaos: Removed after owner ends his turn.
Disease: Never removed (unless cleansed).
Enfeeble: Removed after either player ends his turn.
Fusion: Never removed (unless less than 3 Fusion cards active).
Immobilize: Removed after owner ends his turn.
Infusion: Never removed.
Jam: Removed after owner ends his turn.
Poison: Never removed (unless cleansed).
Protect: Removed before owner begins his turn.
Rally: Removed after owner ends his turn.
Weaken: Removed after owner ends his turn.
Valor: Removed after owner ends his turn.
*/
		Effects[ACTIVATION_JAM] = 0;
		Effects[ACTIVATION_FREEZE] = 0; // is it removed at the end of turn?
		Effects[DMGDEPENDANT_IMMOBILIZE] = 0;
		Effects[ACTIVATION_CHAOS] = 0;
		// really?
		Effects[ACTIVATION_RALLY] = 0;
		Effects[ACTIVATION_WEAKEN] = 0;
	}
	void PlayedCard::Cleanse()
	{
		Effects[DMGDEPENDANT_POISON] = 0;
		Effects[DMGDEPENDANT_DISEASE] = 0;
		Effects[ACTIVATION_JAM] = 0;		
		if (bActivated)  // this is bullshit!
			Effects[ACTIVATION_FREEZE] = 0;
		Effects[DMGDEPENDANT_IMMOBILIZE] = 0;
		Effects[ACTIVATION_ENFEEBLE] = 0;
		Effects[ACTIVATION_CHAOS] = 0;
	}
	bool PlayedCard::IsCleanseTarget()
	{
		// Poison, Disease, Jam, Immobilize, Enfeeble, Chaos. (Does not remove Weaken.)
		return (Effects[DMGDEPENDANT_POISON] || 
				Effects[DMGDEPENDANT_DISEASE] || 
				Effects[ACTIVATION_JAM] ||
				(Effects[ACTIVATION_FREEZE] && bActivated) ||
				Effects[DMGDEPENDANT_IMMOBILIZE] || 
				Effects[ACTIVATION_ENFEEBLE] ||
				Effects[ACTIVATION_CHAOS]);
	}
	void PlayedCard::EndTurn()
	{
		Played(); // for rally
		if ((Wait > 0) && (!Effects[ACTIVATION_FREEZE]))
			DecWait();
	}
	const UCHAR PlayedCard::GetAbilitiesCount() const { return OriginalCard->GetAbilitiesCount(); }
	const UCHAR PlayedCard::GetAbilityInOrder(const UCHAR order) const { return OriginalCard->GetAbilityInOrder(order); }
	void PlayedCard::Infuse(const UCHAR setfaction)
	{
		//OriginalCard.Infuse(setfaction);
		Faction = setfaction;
		SkillProcBuffer[ACTIVATION_INFUSE]++;
	}
    const UCHAR PlayedCard::SufferDmg(UINT QuestEffectId, const UCHAR Dmg, const UCHAR Pierce, UCHAR *actualdamagedealt, UCHAR *SkillProcBuffer, UCHAR *OverkillDamage, bool bCanRegenerate, VLOG *log, LOG_RECORD *lr)
	{
		_ASSERT(OriginalCard);
// Regeneration happens before the additional strikes from Flurry.
// Regenerating does not prevent Crush damage	
		UCHAR dmg = Dmg;
		UCHAR shield = (UCHAR)GetEffect(ACTIVATION_PROTECT);
		if (shield > 0)
		{
			if (Pierce >= shield)
				dmg = Dmg;
			else
			{
				if (Pierce+Dmg >= shield)
					dmg = Pierce + Dmg - shield;
				else
					dmg = 0;
			}
		}
		if (lr)
			lr->Effect = dmg;
		if (dmg >= Health)
		{
			if (OverkillDamage)
				*OverkillDamage += (dmg - Health);
			UCHAR dealt = Health;
			if ((!IsDiseased()) && (bCanRegenerate) && (OriginalCard->GetAbility(DEFENSIVE_REGENERATE))&&(PROC50))
			{
				Health = OriginalCard->GetAbility(DEFENSIVE_REGENERATE);
				fsHealed += OriginalCard->GetAbility(DEFENSIVE_REGENERATE);
				if (QuestEffectId == QEFFECT_INVIGORATE)
					Attack += Health;
				CardSkillProc(DEFENSIVE_REGENERATE);
				if (lr && log)
					log->push_back(LOG_RECORD(lr->Target,DEFENSIVE_REGENERATE,Health));
				if (bConsoleOutput)
				{
					PrintDesc();
					printf(" regenerated %d health\n",Health);
				}
			}
			else
			{
				if (IsAlive()) // shouldn't die twice ;)
					fsDeaths++;
				Health = 0;
				if (lr && log)
					log->push_back(LOG_RECORD(lr->Target,0,0)); // death
				if (bConsoleOutput)
				{
					PrintDesc();
					printf(" died!\n");
				}
			}
			DeathEvents++;
			if (actualdamagedealt) // siphon and leech are kinda bugged - overkill damage counts as full attack damage even if card has 1 hp left, therefore this workaround
			{
				*actualdamagedealt = dealt;
				return dmg;
			}
			// crush damage will be dealt even if the defending unit Regenerates
			return dealt;// note that CRUSH & BACKFIRE are processed elsewhere
		}
		else
		{
			Health -= dmg;
			if (actualdamagedealt)
				*actualdamagedealt = dmg;
			if (bConsoleOutput)
			{
				PrintDesc();
				printf(" suffered %d damage\n",dmg);
			}
		}
		fsDmgMitigated += dmg;
		return dmg;
	}
	
	UCHAR PlayedCard::StrikeDmg(const UINT QuestEffectId, const UCHAR Dmg, UCHAR *overkill) // returns dealt dmg
	{
		_ASSERT(Dmg); // 0 dmg is pointless and indicates an error
		//printf("%s %d <- %d\n",GetName(),GetHealth(),Dmg);
		return SufferDmg(QuestEffectId,Dmg + Effects[ACTIVATION_ENFEEBLE],0,0,0,overkill);
	}
	const bool PlayedCard::IsAlive() const
	{
		return ((OriginalCard) &&/* THIS IS CRAP! */(Health != 0)); //(Attack||Health||Wait);
	}
	const bool PlayedCard::IsDefined() const
	{
		return (OriginalCard && (Attack||Health||Wait||(GetType() == TYPE_COMMANDER)||(GetType() == TYPE_ACTION)));
	}
	bool PlayedCard::OnDeathEvent()
	{
		if (DeathEvents > 0)
		{
			DeathEvents--;
			return true;
		}
		return false;
	}
	const UCHAR PlayedCard::GetRarity() const
	{
        return OriginalCard->GetRarity();
	}
	bool PlayedCard::operator==(const PlayedCard &C) const
	{
		if (OriginalCard != C.OriginalCard) // better to compare ID's, but this should also work
			return false;
		if (Wait != C.Wait)
			return false;
		if (Attack != C.Attack)
			return false;
		if (Health != C.Health)
			return false;
		if (bPlayed != C.bPlayed)
			return false;
		if (bActivated != C.bActivated)
			return false;		
		if (Faction != C.Faction)
			return false;
		return (!memcmp(Effects,C.Effects,CARD_ABILITIES_MAX * sizeof(UCHAR)));
	}
	bool PlayedCard::operator!=(const PlayedCard &C) const
	{
		if (OriginalCard != C.OriginalCard) // better to compare ID's, but this should also work
			return true;
		if (Wait != C.Wait)
			return true;
		if (Attack != C.Attack)
			return true;
		if (Health != C.Health)
			return true;
		if (bPlayed != C.bPlayed)
			return true;
		if (bActivated != C.bActivated)
			return true;
		if (Faction != C.Faction)
			return true;
		return (memcmp(Effects,C.Effects,CARD_ABILITIES_MAX * sizeof(UCHAR)) != 0);
	}
	bool PlayedCard::operator<(const PlayedCard &C) const
	{
		if (OriginalCard != C.OriginalCard) // better to compare ID's, but this should also work
			return (OriginalCard < C.OriginalCard);
		if (Wait != C.Wait)
			return (Wait < C.Wait);
		if (Attack != C.Attack)
			return (Attack < C.Attack);
		if (Health != C.Health)
			return (Health < C.Health);
		if (bPlayed != C.bPlayed)
			return (bPlayed < C.bPlayed);
		if (bActivated != C.bActivated)
			return (bActivated < C.bActivated);		
		if (Faction != C.Faction)
			return (Faction < C.Faction);
		int mr = memcmp(Effects,C.Effects,CARD_ABILITIES_MAX * sizeof(UCHAR)) < 0;
		return (mr < 0) && (mr != 0);
	}
    PlayedCard& PlayedCard::operator=(const Card *card)
	{
		_ASSERT(card);
		OriginalCard = card;
		Attack = card->GetAttack();
		Health = card->GetHealth();
		Wait = card->GetWait();
		Faction = card->GetFaction();
		bQuestSplit = false;
		bPlayed = false;
		bActivated = false;
		memset(Effects,0,CARD_ABILITIES_MAX);
		fsDmgDealt = 0;
		fsDmgMitigated = 0;
		fsAvoided = 0;
		fsHealed = 0;
		fsSpecial = 0;
		fsOverkill = 0;
		fsDeaths = 0;
		SkillProcBuffer = NULL;
		DeathEvents = 0;
        //uniqueId = nextUniqueId++;
        assert(false);
		return *this;
	}
	PlayedCard::PlayedCard(const Card *card)
    : uniqueId(nextUniqueId++)
	{
		_ASSERT(card);
		OriginalCard = card;
		Attack = card->GetAttack();
		Health = card->GetHealth();
		Wait = card->GetWait();
		Faction = card->GetFaction();
		bQuestSplit = false;
		bPlayed = false;
		bActivated = false;
		memset(Effects,0,sizeof(Effects));
		fsDmgDealt = 0;
		fsDmgMitigated = 0;
		fsAvoided = 0;
		fsHealed = 0;
		fsSpecial = 0;
		fsOverkill = 0;
		fsDeaths = 0;
		SkillProcBuffer = NULL;
		DeathEvents = 0;
	}
	const UINT PlayedCard::GetId() const { return OriginalCard->GetId(); }
	const UCHAR PlayedCard::GetNativeAttack() const
	{
		return Attack;
	}
	const UCHAR PlayedCard::GetAttack() const
	{
		char atk = Attack - Effects[ACTIVATION_WEAKEN] + Effects[ACTIVATION_RALLY];
		//printf("[%X] %d = %d - %d + %d\n",this,atk,Attack,Effects[ACTIVATION_WEAKEN],Effects[ACTIVATION_RALLY]);
		if (atk > 0)
			return (UCHAR)atk;
		else
			return 0;
	}
	const UCHAR PlayedCard::GetHealth() const { return Health; }
	const UCHAR PlayedCard::GetMaxHealth() const { return OriginalCard->GetHealth(); }
	const UCHAR PlayedCard::GetFaction() const { return Faction; }
	const UCHAR PlayedCard::GetWait() const { return Wait; }
	const UCHAR PlayedCard::GetType() const
	{
		// this is crap ! - must remove and check the code
		if (!OriginalCard)
			return TYPE_NONE;
		return OriginalCard->GetType();	
	}
	const UCHAR PlayedCard::GetEffect(const UCHAR id) const { return Effects[id]; }
	const EFFECT_ARGUMENT PlayedCard::GetAbility(const UCHAR id) const
	{
		// this is crap ! - must remove and check the code
		if (!OriginalCard) {
            assert(false);
			return 0;
        }
		return this->OriginalCard->GetAbility(id);
	}
	const UCHAR PlayedCard::GetTargetCount(const UCHAR id) const { return OriginalCard->GetTargetCount(id); }
	const UCHAR PlayedCard::GetTargetFaction(const UCHAR id) const
	{
		// this is for infuse
		// if card was infused, we gotta force target faction it was infused into
		if ((Faction != OriginalCard->GetFaction()) && (OriginalCard->GetTargetFaction(id) != FACTION_NONE))// check if card was infused and targetfaction is determined
			return Faction; // force faction it was infused into
		else
			return OriginalCard->GetTargetFaction(id);
	}
	const UCHAR PlayedCard::GetAbilityEvent(const UCHAR id) const { return OriginalCard->GetAbilityEvent(id); }
	const bool PlayedCard::GetPlayed() const { return bPlayed; }
	void PlayedCard::Played() { bPlayed = true; }
	void PlayedCard::ResetPlayedFlag() { bPlayed = false; }
	void PlayedCard::SetAttack(const UCHAR attack) { Attack = attack; }
	void PlayedCard::SetEffect(const UCHAR id, const UCHAR value) { Effects[id] = value; }
	void PlayedCard::Rally(const EFFECT_ARGUMENT amount)
	{
		Effects[ACTIVATION_RALLY] += amount;
		//Attack += amount;
	}
	EFFECT_ARGUMENT PlayedCard::Weaken(const EFFECT_ARGUMENT amount)
	{
		Effects[ACTIVATION_WEAKEN] += amount;
		//if (amount > Attack) Attack = 0; else Attack -= amount;
		return amount; // this is correct and incorrect at the same time ;(
	}
	void PlayedCard::Berserk(const EFFECT_ARGUMENT amount)
	{
		Attack += amount;
	}
	void PlayedCard::Protect(const EFFECT_ARGUMENT amount)
	{
		//if (amount > Effects[ACTIVATION_PROTECT])
		Effects[ACTIVATION_PROTECT] += amount;
	}
	bool PlayedCard::Rush(const EFFECT_ARGUMENT amount)
	{
		if (Wait > 0)
		{
			if (amount >= Wait)
				Wait = 0;
			else
				Wait -= amount;
			return true;
		}
		else
			return false;
	}
	const bool PlayedCard::IsDiseased() const	{	return Effects[DMGDEPENDANT_DISEASE] > 0; }
	UCHAR PlayedCard::Heal(EFFECT_ARGUMENT amount,UINT QuestEffectId)
	{
		_ASSERT(!IsDiseased()); // disallowed
		if (IsDiseased()) return 0;
		if (Health + amount >  OriginalCard->GetHealth())
		{
			amount = (OriginalCard->GetHealth() - Health);
			Health =  OriginalCard->GetHealth();
		}
		else
			Health += amount;
		if (amount && (QuestEffectId == QEFFECT_INVIGORATE))
			Attack += amount;
		return amount;
	}
	const Card * PlayedCard::GetOriginalCard() const { return OriginalCard; }

    PlayedCard::PlayedCard()
    : uniqueId(nextUniqueId++)
	{
		Attack = 0;
		Health = 0;
		Wait = 0;
		Faction = 0;
		bQuestSplit = false;
		bPlayed = false;
		bActivated = false;
		OriginalCard = 0;
		memset(Effects,0,CARD_ABILITIES_MAX);
		fsDmgDealt = 0;
		fsDmgMitigated = 0;
		fsAvoided = 0;
		fsHealed = 0;
		fsSpecial = 0;
		fsOverkill = 0;
		fsDeaths = 0;
		SkillProcBuffer = NULL;
		DeathEvents = 0;
	}

    /**
     * Copy constructor.
     */
    PlayedCard::PlayedCard(PlayedCard const & original)
    : OriginalCard(original.OriginalCard)
    , Attack(original.Attack)
    , Health(original.Health)
    , Wait(original.Wait)
    , Faction(original.Faction)
    , bPlayed(original.bPlayed)
    , bActivated(original.bActivated)
    , bQuestSplit(original.bQuestSplit)
    , DeathEvents(original.DeathEvents)
    , fsDmgDealt(original.fsDmgDealt)
    , fsDmgMitigated(original.fsDmgMitigated)
    , fsAvoided(original.fsAvoided)
    , fsHealed(original.fsHealed)
    , fsSpecial(original.fsSpecial)
    , fsOverkill(original.fsOverkill)
    , fsDeaths(original.fsDeaths)
    , uniqueId(original.uniqueId)
    {
        memcpy(Effects,original.Effects,sizeof(Effects));
        SkillProcBuffer = original.SkillProcBuffer;
    }
#endif

unsigned int PlayedCard::nextUniqueId(0);
struct REQUIREMENT
{
	UCHAR SkillID;
	UCHAR Procs;
	REQUIREMENT() { SkillID = 0; };
};

class ActiveDeck
{
public:
	PlayedCard Commander;
	LCARDS Deck;
	LCARDS Units;
	LCARDS Structures;
	LCARDS Actions;
	//
	bool bOrderMatters;
	MSID Hand;
	bool bDelayFirstCard; // tournament-like
	//
	const UCHAR *CSIndex;
	RESULT_BY_CARD *CSResult;
    /**
     *  Each deck counts the number of times a skill is proced. This buffer will be provided to every PlayedCard in this deck.
     */
	UCHAR SkillProcs[CARD_ABILITIES_MAX];
	//
	UINT CardPicks[DEFAULT_DECK_RESERVE_SIZE];
	UINT CardDeaths[DEFAULT_DECK_RESERVE_SIZE];
	//
	UINT DamageToCommander; // for points calculation - damage dealt to ENEMY commander
	UINT FullDamageToCommander;
	UINT StrongestAttack;
	// logging related stuff
	UCHAR LogDeckID;
	VLOG *Log;
	// Quest effects
	UINT QuestEffectId;
    // used to get the actual card for summon
    Card const * pCDB;
    // logging
    DeckLogger * logger;
public:
    PlayedCard & getUnitAt(unsigned int const index)
    {
        LCARDS::iterator iter = this->Units.begin();
        for(unsigned int i = 0; i < index; i++) {
            iter++;
        }
        return *iter;
    }

    PlayedCard & getCardAt(unsigned int const index)
    {
        LCARDS::iterator iter = this->Deck.begin();
        for(unsigned int i = 0; i < index; i++) {
            iter++;
        }
        return *iter;
    }

    PlayedCard const & getCardAt(unsigned int const index) const
    {
        LCARDS::const_iterator iter = this->Deck.begin();
        for(unsigned int i = 0; i < index; i++) {
            iter++;
        }
        return *iter;
    }
private:
	void Reserve()
	{
		//Deck.reserve(DEFAULT_DECK_RESERVE_SIZE);
		//Units.reserve(DEFAULT_DECK_RESERVE_SIZE);
		//Structures.reserve(DEFAULT_DECK_RESERVE_SIZE);
		//Actions.reserve(DEFAULT_DECK_RESERVE_SIZE);
	}
	LOG_RECORD* LogAdd(LOG_CARD src, UCHAR AbilityID, UCHAR Effect = ABILITY_ENABLED)
	{
		if (!Log) return 0;
		Log->push_back(LOG_RECORD(src,LOG_CARD(),AbilityID,Effect));
		//
		//printf("%d: %d[%d] - %d - = %d\n",src.DeckID,src.DeckID,src.RowID,AbilityID,Effect);
		return &(Log->back());
	}
	LOG_RECORD* LogAdd(LOG_CARD src, LOG_CARD dest, UCHAR AbilityID, UCHAR Effect = ABILITY_ENABLED)
	{
		if (!Log) return 0;
		Log->push_back(LOG_RECORD(src,dest,AbilityID,Effect));
		//
		//printf("%d: %d[%d] - %d -> %d: %d[%d] = %d\n",src.DeckID,src.DeckID,src.RowID,dest.DeckID,dest.DeckID,dest.RowID,AbilityID,Effect);
		return &(Log->back());
	}
	UCHAR GetAliveUnitsCount()
	{
		UCHAR c = 0;
        for(LCARDS::iterator iter=Units.begin(); iter != Units.end(); iter++) {
			if (iter->IsAlive()) {
				c++;
            }
        }
		return c;
	}

// #############################################################################
// #############################################################################
// #############################################################################
// #############################################################################
// #############################################################################
// does anyone know if VALOR procs on commander? imagine combo of valor+flurry or valor+fear
// ok let's assume it does
#define VALOR_HITS_COMMANDER	true

    void AttackCommanderOnce(UCHAR const & index
                            ,PlayedCard & src
                            ,EFFECT_ARGUMENT const & valor
                            ,ActiveDeck & Def
                            ,bool const variant1
                            )
    {
        if(src.GetAttack() <= 0) {
            // can't attack with zero attack, this indicates an error
            throw std::invalid_argument("Zero attack in ActiveDeck::AttackCommanderOnce!");
        }

        if (valor > 0) {
            SkillProcs[COMBAT_VALOR]++;
            LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_VALOR,valor);
        }
        // This should also be fine in variant2.
        if(variant1) {
            if (src.GetAbility(COMBAT_FEAR) && (Def.Units.size() > index) && Def.getUnitAt(index).IsAlive()) {
                SkillProcs[COMBAT_FEAR]++;
                LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_FEAR);
            }
        }
        UCHAR overkill = 0;
        UCHAR cdmg = src.GetAttack()+valor;
        // Update strongest attack
        if (cdmg > StrongestAttack) {
            StrongestAttack = cdmg;
        }

        // do we hit the commander?
        bool const hitCommander(Def.Commander.HitCommander(QuestEffectId
                                                          ,cdmg
                                                          ,src,Def,*this
                                                          ,true,&overkill,Log
                                                          ,LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index)
                                                                 ,LOG_CARD(Def.LogDeckID,TYPE_COMMANDER,0)
                                                                 ,0,cdmg)
                                                          )
                               );
        if (hitCommander) {
            DamageToCommander += cdmg;
            FullDamageToCommander += cdmg;
        }
        if(variant1) {
            src.CardSkillProc(SPECIAL_ATTACK); // attack counter
        }
        // gotta check walls & source onDeath here
        for (LCARDS::iterator iter=Def.Structures.begin(); iter != Def.Structures.end(); iter++) {
            Def.CheckDeathEvents(*iter,*this);
        }
        CheckDeathEvents(src,Def);
        src.fsOverkill += overkill;
        src.fsDmgDealt += cdmg;
        // can go berserk after hitting commander too
        if ((src.GetAttack()+valor > 0) && src.GetAbility(DMGDEPENDANT_BERSERK))
        {
            src.Berserk(src.GetAbility(DMGDEPENDANT_BERSERK));
            LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),DMGDEPENDANT_BERSERK,src.GetAbility(DMGDEPENDANT_BERSERK));
            SkillProcs[DMGDEPENDANT_BERSERK]++;
        }
    }
// #############################################################################
    void AttackCommanderOnce1(UCHAR const & index
                             ,PlayedCard & SRC
                             ,EFFECT_ARGUMENT const & valor
                             ,ActiveDeck & Def
                             )
    {
        AttackCommanderOnce(index, SRC, valor, Def, true);
    }
// #############################################################################
    void AttackCommanderOnce2(UCHAR const & index
                             ,PlayedCard & SRC
                             ,EFFECT_ARGUMENT const & valor
                             ,ActiveDeck & Def
                             )
    {
        AttackCommanderOnce(index, SRC, valor, Def, false);
    }
// #############################################################################
    /**
     * @returns true iff we should continue in the swipe loop
     */
    bool AttackUnitOrCommanderOnce2(PlayedCard & SRC
                                   ,UCHAR const & index
                                   ,PlayedCard & target
                                   ,UCHAR const & targetindex
                                   ,UCHAR const & swipe
                                   ,UCHAR const & s
                                   ,UCHAR & iSwiped
                                   ,ActiveDeck & Def
                                   )
    {
        bool bGoBerserk = false;
        UCHAR burst = 0;
		if ((SRC.GetAbility(COMBAT_BURST)) && (target.GetHealth() == target.GetOriginalCard()->GetHealth()))
			burst = SRC.GetAbility(COMBAT_BURST);
		if (burst > 0)
		{
			SkillProcs[COMBAT_BURST]++;
			LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_BURST,burst);
		}
		// if target dies during flurry and slot(s == 1) is aligned to SRC, we deal dmg to commander
		if (    (!target.IsAlive())
             && ((swipe == 1) || (s == 1))
           )
		{
            if (SRC.GetAttack() > 0) {
                EFFECT_ARGUMENT valor = (VALOR_HITS_COMMANDER && SRC.GetAbility(COMBAT_VALOR) && (GetAliveUnitsCount() < Def.GetAliveUnitsCount())) ? SRC.GetAbility(COMBAT_VALOR) : 0;
                AttackCommanderOnce2(index, SRC, valor, Def);
                // might want to add here check:
                // if (!Def.Commander.IsAlive()) return;
                return true;
            } else {
                return false;
            }
		}
		iSwiped++;
		_ASSERT(target.IsAlive()); // must be alive here
		// actual attack
		// must check valor before every attack
		UCHAR valor = (SRC.GetAbility(COMBAT_VALOR) && (GetAliveUnitsCount() < Def.GetAliveUnitsCount())) ? SRC.GetAbility(COMBAT_VALOR) : 0;
		if (valor > 0)
		{
			SkillProcs[COMBAT_VALOR]++;
			LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_VALOR,valor);
		}
		// attacking flyer
		UCHAR antiair = SRC.GetAbility(COMBAT_ANTIAIR);
		if (target.GetAbility(DEFENSIVE_FLYING))
		{
			if ((!antiair) && (!SRC.GetAbility(DEFENSIVE_FLYING)) && (PROC50 || (QuestEffectId == QEFFECT_HIGH_SKIES))) // missed
			{
				target.fsAvoided += SRC.GetAttack() + valor + burst + target.GetEffect(ACTIVATION_ENFEEBLE); // note that this IGNORES armor and protect
				Def.SkillProcs[DEFENSIVE_FLYING]++;
				LogAdd(LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DEFENSIVE_FLYING);
				return true;
			}
		}
		else antiair = 0; // has no effect if target is not flying
		if (antiair > 0)
		{
			SkillProcs[COMBAT_ANTIAIR]++;
			LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_ANTIAIR,antiair);
		}
		// enfeeble is taken into account before armor
		UCHAR enfeeble = target.GetEffect(ACTIVATION_ENFEEBLE);
		// now armor & pierce
		UCHAR dmg = SRC.GetAttack() + valor + antiair + enfeeble + burst;
		UCHAR armor = target.GetAbility(DEFENSIVE_ARMORED);
		UCHAR pierce = SRC.GetAbility(COMBAT_PIERCE);
		bool bPierce = false;
		if (armor)
		{
			Def.SkillProcs[DEFENSIVE_ARMORED]++;
			LogAdd(LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DEFENSIVE_ARMORED,armor);
			if (pierce > 0)
				bPierce = true;
			if (pierce >= armor)
			{
				armor = 0;
				pierce -= armor; // this is for shield
			}
			else
			{
				armor -= pierce;
				pierce = 0; // this is for shield
			}
			// substract armor from dmg
			if (armor >= dmg)
			{
				target.fsAvoided += dmg;
				dmg = 0;
			}
			else
			{
				target.fsAvoided += armor;
				dmg -= armor; 
			}
		}
		// now we actually deal dmg
		//printf("%s %d = %d => %s %d\n",SRC.GetName(),SRC.GetHealth(),dmg,targets[s]->GetName(),targets[s]->GetHealth());
		if (dmg)
		{
			UCHAR actualdamagedealt = 0;
			bPierce = bPierce || (target.GetShield() && pierce);
			UCHAR overkill = 0;
			if (bPierce)
				LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),COMBAT_PIERCE,pierce);
			dmg = target.SufferDmg(QuestEffectId,dmg, pierce,&actualdamagedealt,0,&overkill,(!SRC.GetAbility(DMGDEPENDANT_DISEASE)),
				Log,LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),0,dmg));
			SRC.fsDmgDealt += actualdamagedealt;
			SRC.fsOverkill += overkill;
		}
		if (bPierce)
			SkillProcs[COMBAT_PIERCE]++;

		// and now dmg dependant effects
		if (!target.IsAlive()) // target just died
		{
			// afaik it ignores walls
			if (target.GetAbility(SPECIAL_BACKFIRE))
			{
				Def.Commander.SufferDmg(QuestEffectId,target.GetAbility(SPECIAL_BACKFIRE));
				DamageToCommander += SRC.GetAbility(SPECIAL_BACKFIRE);
				FullDamageToCommander += SRC.GetAbility(SPECIAL_BACKFIRE);
				Def.SkillProcs[SPECIAL_BACKFIRE]++;
				LogAdd(LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),LOG_CARD(Def.LogDeckID,TYPE_COMMANDER,0),SPECIAL_BACKFIRE,SRC.GetAbility(SPECIAL_BACKFIRE));
			}
			// crush
			if (SRC.GetAbility(DMGDEPENDANT_CRUSH))
			{
				UCHAR overkill = 0;
				if (Def.Commander.HitCommander(QuestEffectId,SRC.GetAbility(DMGDEPENDANT_CRUSH),SRC,Def,*this,false,&overkill,
					Log,LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_COMMANDER,0),DMGDEPENDANT_CRUSH,SRC.GetAbility(DMGDEPENDANT_CRUSH))))
				{
					DamageToCommander += SRC.GetAbility(DMGDEPENDANT_CRUSH);
					FullDamageToCommander += SRC.GetAbility(DMGDEPENDANT_CRUSH);
				}

				// gotta check walls onDeath here
                for (LCARDS::iterator iter=Def.Structures.begin(); iter != Def.Structures.end(); iter++) {
					Def.CheckDeathEvents(*iter,*this);
				}

				SRC.fsOverkill += overkill;
				SkillProcs[DMGDEPENDANT_CRUSH]++;
			}
		}

        // Moraku suggests that "on attack" triggers after crush
        ApplyEffects(QuestEffectId,EVENT_ATTACKED,target,targetindex,*this);

		// counter
		if ((dmg > 0) && target.GetAbility(DEFENSIVE_COUNTER))
		{
			UCHAR overkill = 0;
			UCHAR cdmg = target.GetAbility(DEFENSIVE_COUNTER) + SRC.GetEffect(ACTIVATION_ENFEEBLE);
			target.fsDmgDealt += SRC.SufferDmg(QuestEffectId,cdmg,0,0,0,&overkill); // counter dmg is enhanced by enfeeble
			LogAdd(LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),LOG_CARD(LogDeckID,TYPE_ASSAULT,index),DEFENSIVE_COUNTER,cdmg);
			target.fsOverkill += overkill;
			Def.SkillProcs[DEFENSIVE_COUNTER]++;
			CheckDeathEvents(SRC,Def);
		}
		if (dmg > StrongestAttack)
			StrongestAttack = dmg;
		// berserk
		if ((dmg > 0) && SRC.GetAbility(DMGDEPENDANT_BERSERK))
			bGoBerserk = true;
		// if target is dead, we dont need to process this effects
		if (/*targets[s]->IsAlive() && */(dmg > 0))
		{
			// immobilize
			if (SRC.GetAbility(DMGDEPENDANT_IMMOBILIZE) && PROC50)
			{
				target.SetEffect(DMGDEPENDANT_IMMOBILIZE,SRC.GetAbility(DMGDEPENDANT_IMMOBILIZE));
				SRC.fsSpecial++; // is it good?
				SkillProcs[DMGDEPENDANT_IMMOBILIZE]++;
				LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DMGDEPENDANT_IMMOBILIZE);
			}
			// disease
			if (SRC.GetAbility(DMGDEPENDANT_DISEASE))
			{
				target.SetEffect(DMGDEPENDANT_DISEASE,SRC.GetAbility(DMGDEPENDANT_DISEASE));
				SRC.fsSpecial++; // is it good?
				SkillProcs[DMGDEPENDANT_DISEASE]++;
				LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DMGDEPENDANT_DISEASE);
			}
			// poison
			if (SRC.GetAbility(DMGDEPENDANT_POISON))
				if (target.GetEffect(DMGDEPENDANT_POISON) < SRC.GetAbility(DMGDEPENDANT_POISON)) // overflow
				{
					target.SetEffect(DMGDEPENDANT_POISON,SRC.GetAbility(DMGDEPENDANT_POISON));
					SRC.fsSpecial += SRC.GetAbility(DMGDEPENDANT_POISON); 
					SkillProcs[DMGDEPENDANT_POISON]++;
					LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),LOG_CARD(Def.LogDeckID,TYPE_ASSAULT,targetindex),DMGDEPENDANT_POISON,SRC.GetAbility(DMGDEPENDANT_POISON));
				}
		}
		// leech
		if (SRC.IsAlive() && SRC.GetAbility(DMGDEPENDANT_LEECH))
		{
			UCHAR leech = (SRC.GetAbility(DMGDEPENDANT_LEECH) < dmg) ? SRC.GetAbility(DMGDEPENDANT_LEECH) : dmg;
			if (leech && (!SRC.IsDiseased()))
			{
				leech = SRC.Heal(leech,QuestEffectId);
				SRC.fsHealed += leech;
				if (leech > 0)
				{
					SkillProcs[DMGDEPENDANT_LEECH]++;
					LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),DMGDEPENDANT_LEECH,leech);
				}
			}
		}
		// siphon
		if (SRC.GetAbility(DMGDEPENDANT_SIPHON))
		{
			UCHAR siphon = (SRC.GetAbility(DMGDEPENDANT_SIPHON) < dmg) ? SRC.GetAbility(DMGDEPENDANT_SIPHON) : dmg;
			if (siphon)
			{
				siphon = Commander.Heal(siphon,QuestEffectId);
				SRC.fsHealed += siphon;
				if (siphon > 0)
				{
					SkillProcs[DMGDEPENDANT_SIPHON]++;
					LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),DMGDEPENDANT_SIPHON,siphon);
				}
			}
		}
		if (bGoBerserk)
		{
			SRC.Berserk(SRC.GetAbility(DMGDEPENDANT_BERSERK));
			if (SRC.GetAbility(DMGDEPENDANT_BERSERK))
			{
				SkillProcs[DMGDEPENDANT_BERSERK]++;
				LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),DMGDEPENDANT_BERSERK,SRC.GetAbility(DMGDEPENDANT_BERSERK));
			}
		}

		Def.CheckDeathEvents(target,*this);

		if (!SRC.IsAlive()) // died from counter? during swipe
			return false;
        return true;
    }
// #############################################################################
	void Attack(UCHAR index, ActiveDeck &Def)
	{
        PlayedCard & SRC = this->getUnitAt(index);
		//printf("%s %d\n",SRC.GetName(),SRC.GetHealth());

        // Make sure the attacking unit lives. That should be the case here, thus assertion.
		_ASSERT(SRC.IsDefined() && SRC.IsAlive()); // baby, don't hurt me, no more
		if (bConsoleOutput)
		{
			SRC.PrintDesc();
			printf(" attacks\n");
		}

        // Check for flurry, this is independent of whether there is an unit opposing this or not.
		EFFECT_ARGUMENT iflurry = (SRC.GetAbility(COMBAT_FLURRY) && PROC50) ? (SRC.GetAbility(COMBAT_FLURRY)+1) : 1; // coding like a boss :) don't like this style
		if (iflurry > 1)
		{
			SkillProcs[COMBAT_FLURRY]++;
			LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_FLURRY,iflurry);
            LOG(this->logger,abilityFlurry(SRC,iflurry));
		}

        // Is there no unit opposite of the attacking unit, or do we have fear? ...
        if (   (index >= (UCHAR)Def.Units.size()) // unit is right of everything opponent has
            || (!Def.getUnitAt(index).IsAlive()) // opposite unit is dead
            || (SRC.GetAbility(COMBAT_FEAR)) // unit has fear
           )
        {
            // ... deal DMG to commander directly. BUT STILL PROC50 FLURRY and PROBABLY VALOR
            bool const canValorCardCommander = VALOR_HITS_COMMANDER && SRC.GetAbility(COMBAT_VALOR);
            bool const canValorLessUnits = this->GetAliveUnitsCount() < Def.GetAliveUnitsCount();
            bool const doesValor = canValorCardCommander && canValorLessUnits;
            EFFECT_ARGUMENT const valor = doesValor ? SRC.GetAbility(COMBAT_VALOR) : 0;
			for (UCHAR i=0;i<iflurry;i++)
			{
				AttackCommanderOnce1(index, SRC, valor, Def);
			}
			return; // and thats it!!!
		}
		else // there is a unit
		{
			// oh noes! that wasn't all
			PlayedCard *targets[3];
			// flurry + swipe ...
			UCHAR swipe = (SRC.GetAbility(COMBAT_SWIPE)) ? 3 : 1;
			if (swipe > 1) {
				if ((index > 0) && (Def.getUnitAt(index-1).IsAlive())) {
					targets[0] = &Def.getUnitAt(index-1);
				} else {
					targets[0] = 0;
                }
				targets[1] = &Def.getUnitAt(index);
				_ASSERT(targets[1]); // this is aligned to SRC and must be present
				if ((index+1 < (UCHAR)Def.Units.size()) && (Def.getUnitAt(index+1).IsAlive())) {
					targets[2] = &Def.getUnitAt(index+1);
				} else {
					targets[2] = 0;
                }
				LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,index),COMBAT_SWIPE);
			} else {
				targets[0] = &Def.getUnitAt(index);
			}
			// flurry
			for (UCHAR iatk=0;iatk<iflurry;iatk++)
			{
				UCHAR iSwiped = 0;
				//
				SRC.CardSkillProc(SPECIAL_ATTACK); // attack counter
				// swipe
				for (UCHAR s=0;s<swipe;s++)
				{
					if (
						((!s) || (s == 2)) &&
						((!targets[s]) || ( (!targets[s]->IsAlive()) && ((s != 1) && (swipe != 1)) ))
						) // empty slot to the left or right, swipe doesnt proc, if swipe == 1 then targets[0] can't be null
						continue;
					UCHAR targetindex = index;
					if (swipe > 1)
					{
						targetindex += s;
						targetindex--;
					}
					bool doContinue = AttackUnitOrCommanderOnce2(SRC, index, *targets[s], targetindex, swipe,s, iSwiped, Def);
					if(doContinue) {continue;} else {break;}
				} // end of swipe
				if (iSwiped > 1)
					SkillProcs[COMBAT_SWIPE]++;
				if (!SRC.IsAlive()) // died from counter? during flurry
					break;
			} // end of flurry
		} // end of "not hit commander directly"
#undef SRC
	}
// #############################################################################
// #############################################################################
// #############################################################################
// #############################################################################
// #############################################################################

public:
	ActiveDeck() 
	{
		QuestEffectId = 0;
		Log = 0;
		bOrderMatters = false; 
		bDelayFirstCard = false; 
		CSIndex = 0; 
		CSResult = 0; 
		DamageToCommander = 0; 
		FullDamageToCommander = 0;
		StrongestAttack = 0;
		memset(SkillProcs,0,sizeof(SkillProcs));
		memset(CardPicks,0,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
		memset(CardDeaths,0,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
		pCDB = NULL;
	}
	~ActiveDeck() { Deck.clear(); Units.clear(); Structures.clear(); Actions.clear(); }
public:
#define REQ_MAX_SIZE			5
	bool CheckRequirements(const REQUIREMENT *Reqs)
	{
		if (!Reqs) return true;
		for (UCHAR i=0;(i<REQ_MAX_SIZE) && (Reqs[i].SkillID);i++)
			if (SkillProcs[Reqs[i].SkillID] < Reqs[i].Procs)
				return false;
		return true;
	}
	bool IsAnnihilated()
	{
		if (!Deck.empty()) return false;
		for (LCARDS::iterator vi = Units.begin(); vi != Units.end(); vi++)
			if (vi->IsAlive())
				return false;
		for (LCARDS::iterator vi = Structures.begin(); vi != Structures.end(); vi++)
			if (vi->IsAlive())
				return false;
		return true;
	}
	void SetFancyStatsBuffer(const UCHAR *resindex, RESULT_BY_CARD *res)
	{
		CSIndex = resindex;
		CSResult = res;
	}
	UCHAR GetCountInDeck(UINT Id)
	{
		if (Commander.GetId() == Id)
			return 1;
		UCHAR c = 0;
		for (LCARDS::iterator vi = Deck.begin(); vi != Deck.end(); vi++)
			if (vi->GetId() == Id)
				c++;
		return c;
	}
	void SetQuestEffect(UINT EffectId)
	{
		QuestEffectId = EffectId;
	}
	// please note, contructors don't clean up storages, must do it manually and beforehand, even copy constructor
	ActiveDeck(const char *HashBase64, const Card *pCDB)
	: pCDB(pCDB)
	{
		_ASSERT(pCDB);
		_ASSERT(HashBase64);
		QuestEffectId = 0;
		Log = 0;
		CSIndex = 0;
		CSResult = 0;
		DamageToCommander = 0;
		FullDamageToCommander = 0;
		StrongestAttack = 0;
		bOrderMatters = false; 
		bDelayFirstCard = false;
		unsigned short tid = 0, lastid = 0;
		memset(SkillProcs,0,sizeof(SkillProcs));
		memset(CardPicks,0,sizeof(CardPicks));
		memset(CardDeaths,0,sizeof(CardDeaths));
		//
		size_t len = strlen(HashBase64);
        if(len % 2 != 0) {
            throw InvalidDeckHashError(InvalidDeckHashError::notEvenChars);
        }
		_ASSERT(!(len & 1)); // bytes should go in pairs
		if (len & 1)
			return;
		len = len >> 1; // div 2
		//Deck.reserve(DEFAULT_DECK_RESERVE_SIZE);
		for (UCHAR i = 0; i < len; i++)
		{
			if (HashBase64[i << 1] == '.') break; // delimeter
			if (isspace(HashBase64[i << 1])) break; // not a hash
			tid = BASE64ID((HashBase64[i << 1] << 8) + HashBase64[(i << 1) + 1]);
			if (i==0)
			{
			    // first card is commander
				_ASSERT(tid < CARD_MAX_ID);
				_ASSERT((tid >= 1000) && (tid < 2000)); // commander Id boundaries
				Commander = PlayedCard(&pCDB[tid]);
				Commander.SetCardSkillProcBuffer(SkillProcs);
			}
			else
			{
			    // later cards are not commander
				_ASSERT(i || (tid < CARD_MAX_ID)); // commander card can't be encoded with RLE
				if (tid < CARD_MAX_ID)
				{
				    // this is a card
					Deck.push_back(&pCDB[tid]);
					lastid = tid;
				}
				else
				{
				    // this is an encoding for rle
					for (UINT k = CARD_MAX_ID+1; k < tid; k++) // decode RLE, +1 because we already added one card
						Deck.push_back(&pCDB[lastid]);
				}
			}
		}
	}
	ActiveDeck(const Card *Cmd)
	{ 
		QuestEffectId = 0;
		Log = 0;
		bOrderMatters = false; 
		bDelayFirstCard = false; 
		CSIndex = 0; 
		CSResult = 0;
		DamageToCommander = 0; 
		FullDamageToCommander = 0;
		StrongestAttack = 0;
		Commander = PlayedCard(Cmd); 
		Commander.SetCardSkillProcBuffer(SkillProcs); 
		//Deck.reserve(DEFAULT_DECK_RESERVE_SIZE); 
		memset(SkillProcs,0,sizeof(SkillProcs));
		memset(CardPicks,0,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
		memset(CardDeaths,0,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
	};
	ActiveDeck(const ActiveDeck &D) // need copy constructor
	: pCDB(D.pCDB)
    , logger(D.logger)
	{
		QuestEffectId = D.QuestEffectId;
		Log = 0;
		Commander = D.Commander;
		Commander.SetCardSkillProcBuffer(SkillProcs);
		//Deck.reserve(D.Deck.size());
		Deck = D.Deck;
		//Actions.reserve(D.Actions.size());
        Actions = D.Actions;
		for (LCARDS::iterator iter = Actions.begin(); iter != Actions.end(); iter++) {
			iter->SetCardSkillProcBuffer(SkillProcs); // have to reassign buffers
		}
		//Units.reserve(D.Units.size());
        Units = D.Units;
		for (LCARDS::iterator iter = Units.begin(); iter != Units.end(); iter++) {
			iter->SetCardSkillProcBuffer(SkillProcs); // have to reassign buffers
		}
		//Structures.reserve(D.Structures.size());
        Structures = D.Structures;
        for (LCARDS::iterator iter = Actions.begin(); iter != Actions.end(); iter++) {
			iter->SetCardSkillProcBuffer(SkillProcs); // have to reassign buffers
		}
        bOrderMatters = D.bOrderMatters;
		bDelayFirstCard = D.bDelayFirstCard;
		memcpy(SkillProcs,D.SkillProcs,sizeof(SkillProcs));
		memcpy(CardPicks,D.CardPicks,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
		memcpy(CardDeaths,D.CardDeaths,DEFAULT_DECK_RESERVE_SIZE*sizeof(UINT));
		CSIndex = D.CSIndex;
		CSResult = D.CSResult;
		DamageToCommander = D.DamageToCommander;
		FullDamageToCommander = D.FullDamageToCommander;
		StrongestAttack = D.StrongestAttack;
		if (D.bOrderMatters)
		{
			Hand.clear();
			for (MSID::iterator si=D.Hand.begin();si!=D.Hand.end();si++)
				Hand.insert(*si);			
		}
		//for (VCARDS::iterator vi = D.Deck.begin();vi != D.Deck.end();vi++)
		//	Deck.push_back(*vi);
	}
	bool operator==(const ActiveDeck &D) const
	{
		if (strcmp(GetHash64().c_str(),D.GetHash64().c_str()))
			return false;
        if (Units != D.Units) {
            return false;
        }
        if (Structures != D.Structures) {
            return false;
        }
        if (Actions != D.Actions) {
            return false;
        }
		return true;
	}
	bool operator<(const ActiveDeck &D) const
	{
		int sr = strcmp(GetHash64().c_str(),D.GetHash64().c_str());
		if (sr)
			return (sr < 0);
		if (Units.size() != D.Units.size())
			return (Units.size() < D.Units.size());
        for (LCARDS::const_iterator i = Units.begin(), j = D.Units.begin()
            ; i != Units.end()
            ; i++,j++
            ) {
            if(*i != *j) {
                return *i < *j;
            }
        }
		if (Structures.size() != D.Structures.size())
			return (Structures.size() < D.Structures.size());
        for (LCARDS::const_iterator i = Structures.begin(), j = D.Structures.begin()
            ; i != Structures.end()
            ; i++,j++
            ) {
            if(*i != *j) {
                return *i < *j;
            }
        }
		if (Actions.size() != D.Actions.size())
			return (Actions.size() < D.Actions.size());
        for (LCARDS::const_iterator i = Actions.begin(), j = D.Actions.begin()
            ; i != Actions.end()
            ; i++,j++
            ) {
            if(*i != *j) {
                return *i < *j;
            }
        }
		return false;
	}
	const bool IsValid(bool bSoftCheck = false) const
	{
		if (!Commander.IsDefined())
			return false;
		if (Deck.empty())
			return true;
		if (bSoftCheck)
			return true;
		set <UINT> cards;
		bool bLegendary = false;
		for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++)
		{
			UINT rarity = iter->GetRarity();
			if (rarity == RARITY_LEGENDARY)
			{
				if (bLegendary)
					return false;
				else
					bLegendary = true;
			} else {
				if (iter->GetRarity() == RARITY_UNIQUE)
				{
					if (cards.find(iter->GetId()) != cards.end())
						return false;
					else
						cards.insert(iter->GetId());
				}
			}
		}
		return true;
	}
	void SetOrderMatters(const bool bMatters)
	{
		bOrderMatters = bMatters;
	}
	void DelayFirstCard()
	{
		bDelayFirstCard = true;
	}
	void Add(const Card *c)
	{
		Deck.push_back(c);
	}
	bool IsInTargets(PlayedCard *pc, PPCIV *targets) // helper
	{
		for (PPCIV::iterator vi=targets->begin();vi!=targets->end();vi++)
			if (pc == vi->first)
				return true;
		return false;
	}
	UCHAR Intercept(PPCIV &targets, UCHAR destindex, ActiveDeck &Dest)
	{
		if (targets.size() < 2)
			return destindex;
		// modify a target by intercept here
		if ((destindex > 0) &&
			(targets[destindex-1].second == targets[destindex].second - 1) && 
			(targets[destindex-1].first->GetAbility(DEFENSIVE_INTERCEPT) > 0))
		{
			destindex = destindex-1;
			Dest.SkillProcs[DEFENSIVE_INTERCEPT]++;
			LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,destindex),DEFENSIVE_INTERCEPT);
		}
		else
			if ((destindex < targets.size() - 1) &&
				(targets[destindex+1].second == targets[destindex].second + 1) && 
				(targets[destindex+1].first->GetAbility(DEFENSIVE_INTERCEPT) > 0))
			{
				destindex = destindex+1;
				Dest.SkillProcs[DEFENSIVE_INTERCEPT]++;
				LogAdd(LOG_CARD(LogDeckID,TYPE_ASSAULT,destindex),DEFENSIVE_INTERCEPT);
			}
		return destindex;
	}
	void ApplyEffects(UINT QuestEffectId,EVENT_CONDITION EffectType, PlayedCard &Src,int Position,ActiveDeck &Dest,bool IsMimiced=false,bool IsFusioned=false,PlayedCard *Mimicer=0,UCHAR StructureIndex = 0)
	{
		UCHAR destindex,aid,faction;
		PPCIV targets;
		targets.reserve(DEFAULT_DECK_RESERVE_SIZE);
		PPCARDINDEX tmp;
		EFFECT_ARGUMENT effect;
		UCHAR FusionMultiplier = 1;
		if (IsFusioned)
			FusionMultiplier = 2;

		bool bSplit = false;

		UCHAR SrcPos = StructureIndex;
		if (Position > 0)
			SrcPos = (UCHAR)Position;

		bool bIsSelfMimic = false; // Chaosed Mimic indicator
		if (IsMimiced && Mimicer && (!Units.empty()))
		{
            for (LCARDS::iterator iter = Deck.begin(); iter != Deck.end(); iter++)
                // FIXME: Intended asssigment? I think not!
				if (Mimicer = &(*iter))
				{
					bIsSelfMimic = true;
					break;
				}
		}
		if ((QuestEffectId == QEFFECT_CLONE_PROJECT) && (!IsMimiced) && (Src.GetQuestSplit()) && (EffectType == EVENT_EMPTY))
		{
			Src.SetQuestSplit(false); // remove mark
			bSplit = true;
		}
		// here is a good question - can paybacked skill be paybacked? - nope
		// can paybacked skill be evaded? - doubt
		// in current model, it can't be, payback applies effect right away, without simulationg it's cast
		// another question is - can paybacked skill be evaded? it is possible, but in this simulator it can't be
		// both here and in branches
		UCHAR ac = Src.GetAbilitiesCount();
		if ((!ac) && (QuestEffectId == QEFFECT_TIME_SURGE) && (!IsMimiced) && (EffectType == EVENT_EMPTY))
			ac = 1;
		for (UCHAR aindex=0;aindex<ac;aindex++)
		{
			if (!IsMimiced)
			{
				if (Src.GetEffect(ACTIVATION_JAM) > 0)
					break; // card was jammed by payback (or chaos?)
				if (Src.GetEffect(ACTIVATION_FREEZE) > 0)
					break; // chaos-mimic-freeze makes this possible
			}
			UCHAR chaos = Src.GetEffect(ACTIVATION_CHAOS); // I need to check this every time card uses skill because it could be paybacked chaos - such as Pulsifier with payback, chaos and mimic

			aid = Src.GetAbilityInOrder(aindex);

			// filter certain types of skills
			/*EMPTY - EMPTY
			DIED - DIED
			DIED - BOTH
			PLAY - PLAY
			PLAY - BOTH*/
			EVENT_CONDITION AbilityEventType(Src.GetAbilityEvent(aid));
			// in general: we only continue if the event type for the skill is the same as the event we process
			// EMPTY is a special case...
			if (EffectType == EVENT_EMPTY && AbilityEventType != EVENT_EMPTY) {
			    // ... this is a normal (empty) event handling run, but the card has something different
			    continue;
			}
			// ... for non empty we use binary and
			if (   EffectType != EVENT_EMPTY
			    && ((EffectType & AbilityEventType) == 0)
			   ) {
			    // ... this is a non-normal event handling run, but the card does not have this event
			    continue;
			}

			// cleanse
			if ((aid == ACTIVATION_CLEANSE) && (QuestEffectId != QEFFECT_DECAY))
			{
				effect = Src.GetAbility(aid) * FusionMultiplier; // will it be fusioned? who knows
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					GetTargets(Units,faction,targets);
					LOG_CARD lc(LogDeckID,TYPE_ASSAULT,100);
					if (targets.size())
					{
						PPCIV::iterator vi = targets.begin();
						while (vi != targets.end())
						{
							if (!vi->first->IsCleanseTarget())
								vi = targets.erase(vi);
							else vi++;
						}
						bool bTributable = (IsMimiced && IsInTargets(Mimicer,&targets)) || ((!IsMimiced) && IsInTargets(&Src,&targets));
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
						{
							destindex = UCHAR(rand() % targets.size());
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						for (vi = targets.begin();vi != targets.end();vi++)
						{
							if (bConsoleOutput)
							{
								Src.PrintDesc();
								printf(" cleanse ");
								vi->first->PrintDesc();
								printf("\n");
							}
							lc.CardID = vi->second;
							vi->first->Cleanse();
							if (!IsMimiced)
							{
								Src.fsSpecial += effect;
								LogAdd(LOG_CARD(LogDeckID,Src.GetType(),SrcPos),lc,aid);
							}
							else
							{
								Mimicer->fsSpecial += effect;
								LogAdd(LOG_CARD(LogDeckID,Mimicer->GetType(),SrcPos),lc,aid);
							}
							if (vi->first->GetAbility(DEFENSIVE_TRIBUTE) && bTributable)
							{
								if (IsMimiced)
								{
									if ((Mimicer->GetType() == TYPE_ASSAULT) && (Mimicer != vi->first) && PROC50)
									{
										Dest.SkillProcs[DEFENSIVE_TRIBUTE]++;
										Mimicer->Cleanse();
										LogAdd(lc,DEFENSIVE_TRIBUTE);
										LogAdd(lc,LOG_CARD(LogDeckID,Mimicer->GetType(),SrcPos),aid);
										vi->first->fsSpecial += effect;
									}
								}
								else
								{
									if ((Src.GetType() == TYPE_ASSAULT) && (&Src != vi->first) && PROC50)
									{
										SkillProcs[DEFENSIVE_TRIBUTE]++;
										Src.Cleanse();
										LogAdd(lc,DEFENSIVE_TRIBUTE);
										LogAdd(lc,LOG_CARD(LogDeckID,Src.GetType(),SrcPos),aid);
										vi->first->fsSpecial += effect;
									}
								}
							}
						}
					}
					targets.clear();
				}
			}
			// enfeeble
			if (aid == ACTIVATION_ENFEEBLE)
			{
				effect = Src.GetAbility(aid) * FusionMultiplier;
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					LOG_CARD lc(LogDeckID,TYPE_ASSAULT,100);
					if (chaos > 0)
					{
						GetTargets(Units,faction,targets);
						lc.DeckID = LogDeckID;
					}
					else
					{
						GetTargets(Dest.Units,faction,targets);
						lc.DeckID = Dest.LogDeckID;
					}
					if (targets.size())
					{
						if (Src.GetTargetCount(aid) != TARGETSCOUNT_ALL)
						{
							destindex = UCHAR(rand() % targets.size());
							if (!chaos)
								destindex = Intercept(targets, destindex, Dest);
							tmp = targets[destindex];							
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
							if ((vi->first->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && (!chaos) && (PROC50))
							{
								// evaded
								Dest.SkillProcs[DEFENSIVE_EVADE]++;
								LogAdd(LOG_CARD(Dest.LogDeckID,vi->first->GetType(),vi->second),lc,aid);
							}
							else
							{
								lc.CardID = vi->second;
								vi->first->SetEffect(aid,vi->first->GetEffect(aid) + effect);
								if (!IsMimiced)
								{
									Src.fsSpecial += effect;
									LogAdd(LOG_CARD(LogDeckID,Src.GetType(),SrcPos),lc,aid,effect);
								}
								else
								{
									Mimicer->fsSpecial += effect;
									LogAdd(LOG_CARD(Dest.LogDeckID,Mimicer->GetType(),SrcPos),lc,aid,effect);
								}
								if (Src.IsAlive() && (EffectType != EVENT_DIED))
									if (vi->first->GetAbility(DEFENSIVE_PAYBACK) && (Src.GetType() == TYPE_ASSAULT) && PROC50 && (!chaos))  // payback
									{
										Src.SetEffect(aid,Src.GetEffect(aid) + effect);
										vi->first->fsSpecial += effect;
										Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
									}
								if (bConsoleOutput)
								{
									Src.PrintDesc();
									printf(" enfeeble ");
									vi->first->PrintDesc();
									printf(" for %d\n",effect);
								}
							}
					}
					targets.clear();
				}
			}
			// heal
			if (aid == ACTIVATION_HEAL)
			{
				effect = Src.GetAbility(aid) * FusionMultiplier;
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					GetTargets(Units,faction,targets);
					if (targets.size())
					{
						PPCIV::iterator vi = targets.begin();
						while (vi != targets.end())
						{
							if ((vi->first->GetHealth() == vi->first->GetMaxHealth()) || (vi->first->IsDiseased()))
								vi = targets.erase(vi);
							else vi++;
						}
						bool bTributable = (IsMimiced && IsInTargets(Mimicer,&targets)) || ((!IsMimiced) && IsInTargets(&Src,&targets));
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
						{
							destindex = UCHAR(rand() % targets.size());
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						for (vi = targets.begin();vi != targets.end();vi++)
						{
							if (bConsoleOutput)
							{
								Src.PrintDesc();
								printf(" heals ");
								vi->first->PrintDesc();
								printf(" for %d\n",effect);
							}
							if (!IsMimiced)
								Src.fsHealed += vi->first->Heal(effect,QuestEffectId);
							else
								Mimicer->fsHealed += vi->first->Heal(effect,QuestEffectId);
							if (vi->first->GetAbility(DEFENSIVE_TRIBUTE) && bTributable && PROC50)
							{
								if (IsMimiced)
								{
									if ((Mimicer->GetType() == TYPE_ASSAULT) && (Mimicer != vi->first))
									{
										Dest.SkillProcs[DEFENSIVE_TRIBUTE]++;
										vi->first->fsHealed += Mimicer->Heal(effect,QuestEffectId);
									}
								}
								else
								{
									if ((Src.GetType() == TYPE_ASSAULT) && (&Src != vi->first))
									{
										SkillProcs[DEFENSIVE_TRIBUTE]++;
										vi->first->fsHealed += Src.Heal(effect,QuestEffectId);
									}
								}
							}
						}
					}
					targets.clear();
				}
			}
			// supply
			if (aid == ACTIVATION_SUPPLY)
			{
				effect = Src.GetAbility(aid) * FusionMultiplier;
				if ((effect > 0) && (Position >= 0) && ((!IsMimiced) || (Mimicer && (Mimicer->GetType() == TYPE_ASSAULT)))) // can only be mimiced by assault cards
				{
					targets.clear();
					if (Position)
						targets.push_back(PPCARDINDEX(&(this->getUnitAt(Position-1)),Position-1));
					targets.push_back(PPCARDINDEX(&(this->getUnitAt(Position)),Position));
					if ((DWORD)Position+1 < Units.size())
						targets.push_back(PPCARDINDEX(&(this->getUnitAt(Position+1)),Position+1));
					if (targets.size())
					{
						PPCIV::iterator vi = targets.begin();
						while (vi != targets.end())
						{
							if ((vi->first->GetHealth() == vi->first->GetMaxHealth()) || vi->first->IsDiseased())
								vi = targets.erase(vi);
							else vi++;
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						//FIXME: That variable is unused, yet is has a large right hand side...
						bool bTributable = (IsMimiced && IsInTargets(Mimicer,&targets)) || ((!IsMimiced) && IsInTargets(&Src,&targets));
						for (vi = targets.begin();vi != targets.end();vi++)
						{
							if (bConsoleOutput)
							{
								Src.PrintDesc();
								printf(" supply ");
								vi->first->PrintDesc();
								printf(" for %d\n",effect);
							}
							if (!IsMimiced)
								Src.fsHealed += vi->first->Heal(effect,QuestEffectId);
							else
								Mimicer->fsHealed += vi->first->Heal(effect,QuestEffectId);
							if (vi->first->GetAbility(DEFENSIVE_TRIBUTE) && PROC50)
							{
								if (IsMimiced)
								{
									if ((Mimicer->GetType() == TYPE_ASSAULT) && (Mimicer != vi->first))
									{
										Dest.SkillProcs[DEFENSIVE_TRIBUTE]++;
										vi->first->fsHealed += Mimicer->Heal(effect,QuestEffectId);
									}
								}
								else
								{
									if ((Src.GetType() == TYPE_ASSAULT) && (&Src != vi->first))
									{
										SkillProcs[DEFENSIVE_TRIBUTE]++;
										vi->first->fsHealed += Src.Heal(effect,QuestEffectId);
									}
								}
							}
						}
					}
					targets.clear();
				}
			}
			// protect
			// do skills like PROTECT ALL exist? in case they do - does shield override another shield?
			if (aid == ACTIVATION_PROTECT)
			{
				effect = Src.GetAbility(aid) * FusionMultiplier; // will it be fusioned? who knows
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					GetTargets(Units,faction,targets);
					if (targets.size())
					{
						PPCIV::iterator vi = targets.begin();
						/*if (Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) // if it is not PROTECT ALL then remove from targets already shielded cards
							while (vi != targets.end())
							{
								if ((*vi)->GetShield() > 0) // already shielded
									vi = targets.erase(vi);
								else vi++;
							}*/
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
						{
							destindex = UCHAR(rand() % targets.size());
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						for (vi = targets.begin();vi != targets.end();vi++)
						{
							if (bConsoleOutput)
							{
								Src.PrintDesc();
								printf(" protects ");
								vi->first->PrintDesc();
								printf(" for %d\n",effect);
							}
							vi->first->Protect(effect);
							if (!IsMimiced)
								Src.fsSpecial += effect;
							else
								Mimicer->fsSpecial += effect;
							if (vi->first->GetAbility(DEFENSIVE_TRIBUTE) && PROC50)
							{
								if (IsMimiced)
								{
									if ((Mimicer->GetType() == TYPE_ASSAULT) && (Mimicer != vi->first))
									{
										Mimicer->Protect(effect);
										vi->first->fsSpecial += effect;
										Dest.SkillProcs[DEFENSIVE_TRIBUTE]++;
									}
								}
								else
								{
									if ((Src.GetType() == TYPE_ASSAULT) && (&Src != vi->first))
									{
										Src.Protect(effect);
										vi->first->fsSpecial += effect;
										SkillProcs[DEFENSIVE_TRIBUTE]++;
									}
								}
							}
						}
					}
					targets.clear();
				}
			}
			// infuse is processed on the upper level
			// ******
			// jam
			if (aid == ACTIVATION_JAM)
			{
				effect = Src.GetAbility(aid);
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					if (chaos > 0)
						GetTargets(Units,faction,targets);
					else
						GetTargets(Dest.Units,faction,targets);
					if (targets.size())
					{
						PPCIV::iterator vi = targets.begin();
						while (vi != targets.end())
						{
							if ((vi->first->GetEffect(aid)) || (vi->first->GetWait()))
								vi = targets.erase(vi);
							else vi++;
						}
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
						{
							destindex = UCHAR(rand() % targets.size());
							if (!chaos)
								destindex = Intercept(targets, destindex, Dest);
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
						{
							if (PROC50)
							{
								if ((vi->first->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && (PROC50) && (!chaos))
								{
									// evaded
									// no fancy stats here?!
									Dest.SkillProcs[DEFENSIVE_EVADE]++;
								}
								else
								{
									vi->first->SetEffect(aid,effect);
									if (!IsMimiced)
										Src.fsSpecial += effect;
									else
										Mimicer->fsSpecial += effect;
									if ((!IsMimiced) || bIsSelfMimic)
										SkillProcs[aid]++;
									else
										Dest.SkillProcs[aid]++;
									if (Src.IsAlive() && (EffectType != EVENT_DIED))
			/*  ?  */					if (vi->first->GetAbility(DEFENSIVE_PAYBACK) && (Src.GetType() == TYPE_ASSAULT) && PROC50 && (!chaos))  // payback is it 1/2 or 1/4 chance to return jam with payback????
										{
											Src.SetEffect(aid,effect);
											vi->first->fsSpecial += effect;
											Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
										}
									if (bConsoleOutput)
									{
										Src.PrintDesc();
										printf(" jam ");
										vi->first->PrintDesc();
										printf("\n");
									}
								}
							}
						}
					}
					targets.clear();
				}
			}
			// freeze
			if (aid == ACTIVATION_FREEZE)
			{
				effect = Src.GetAbility(aid);
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					if (chaos > 0)
						GetTargets(Units,faction,targets);
					else
						GetTargets(Dest.Units,faction,targets);
					if (targets.size())
					{
						PPCIV::iterator vi = targets.begin();
						while (vi != targets.end())
						{
							if ((vi->first->GetEffect(aid)))
								vi = targets.erase(vi);
							else vi++;
						}
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
						{
							destindex = UCHAR(rand() % targets.size());
							if (!chaos)
								destindex = Intercept(targets, destindex, Dest);
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
							if ((vi->first->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && (PROC50) && (!chaos))
							{
								// evaded
								// no fancy stats here?!
								Dest.SkillProcs[DEFENSIVE_EVADE]++;
							}
							else
							{
								vi->first->SetEffect(aid,effect);
								if (!IsMimiced)
									Src.fsSpecial += effect;
								else
									Mimicer->fsSpecial += effect;
								if (Src.IsAlive() && (EffectType != EVENT_DIED))
									if (vi->first->GetAbility(DEFENSIVE_PAYBACK) && (Src.GetType() == TYPE_ASSAULT) && PROC50 && (!chaos)) 
									{
										Src.SetEffect(aid,effect);
										vi->first->fsSpecial += effect;
										Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
									}
								if (bConsoleOutput)
								{
									Src.PrintDesc();
									printf(" freeze ");
									vi->first->PrintDesc();
									printf("\n");
								}
							}
					}
					targets.clear();
				}
			}
			// mimic - could be tricky
			if (aid == ACTIVATION_MIMIC)
			{
				effect = Src.GetAbility(aid);
				if ((effect > 0) && (!IsMimiced))
				{
					if (IsMimiced) // mimic can't be mimiced ;) it's just sad copypaste, previous line prevents this being mimiced
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					if ((chaos > 0) || (QuestEffectId == QEFFECT_COPYCAT))
						GetTargets(Units,faction,targets);
					else
						GetTargets(Dest.Units,faction,targets);
					if (targets.size())
					{
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL))
						{
							destindex = UCHAR(rand() % targets.size());
							// can Mimic be intercepted?
							if (!chaos)
								destindex = Intercept(targets, destindex, Dest);
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
							SkillProcs[aid]++;
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
							if ((vi->first->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && (PROC50) && (!chaos))
							{
								// evaded
								Dest.SkillProcs[DEFENSIVE_EVADE]++;
							}
							else
							{
								if (bConsoleOutput)
								{
									Src.PrintDesc();
									printf(" mimic ");
									vi->first->PrintDesc();
									printf("\n");
								}
								if (chaos > 0)
									ApplyEffects(QuestEffectId,EVENT_EMPTY,*vi->first,Position,*this,true,false,&Src);
								else
									ApplyEffects(QuestEffectId,EVENT_EMPTY,*vi->first,Position,Dest,true,false,&Src);	
							}
					}
					targets.clear();
				}
			}
			// rally
			// does rally prefer units with 0 attack? ???? or its completely random
			// afaik it's completely random
			if (aid == ACTIVATION_RALLY)
			{
				effect = Src.GetAbility(aid) * FusionMultiplier;
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					GetTargets(Units,faction,targets);
                    // If we have targets at all
                    if (targets.size() > 0) {
                        PPCIV::iterator vi(targets.begin());
                        // look at each target, remove it if it can't attack
                        while (vi != targets.end()) {
                            if (    (vi->first->GetWait() > 0)  // only rally units that are ready
                                 || (vi->first->GetPlayed())    // don't rally units that already did their action
                                 || (vi->first->GetEffect(ACTIVATION_JAM)) // Jammed
                                 || (vi->first->GetEffect(ACTIVATION_FREEZE)) // Frozen
                                 || (vi->first->GetEffect(DMGDEPENDANT_IMMOBILIZE))   // Immobilized
                               ) {
                                vi = targets.erase(vi);
                            } else {
                                vi++;
                            }
                        } // while
						bool bTributable = (IsMimiced && IsInTargets(Mimicer,&targets)) || ((!IsMimiced) && IsInTargets(&Src,&targets));
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
						{
							destindex = UCHAR(rand() % targets.size());
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
						{
							if (bConsoleOutput)
							{
								Src.PrintDesc();
								printf(" rally ");
								vi->first->PrintDesc();
								printf(" for %d\n",effect);
							}
							vi->first->Rally(effect);		
							if (!IsMimiced)
								Src.fsSpecial += effect;
							else
								Mimicer->fsSpecial += effect;
							if (vi->first->GetAbility(DEFENSIVE_TRIBUTE) && bTributable && PROC50)
							{
								if (IsMimiced)
								{
									if ((Mimicer->GetType() == TYPE_ASSAULT) && (Mimicer != vi->first))
									{
										Mimicer->Rally(effect);
										vi->first->fsSpecial += effect;
										Dest.SkillProcs[DEFENSIVE_TRIBUTE]++;
									}
								}
								else
								{
									if ((Src.GetType() == TYPE_ASSAULT) && (&Src != vi->first))
									{
										Src.Rally(effect);
										vi->first->fsSpecial += effect;
										SkillProcs[DEFENSIVE_TRIBUTE]++;										
									}
								}
							}
						}
					} // if we have targets at all
					targets.clear();
				}
			}
			// recharge -  only action cards
			if (aid == ACTIVATION_RECHARGE)
			{
				if (Src.GetAbility(aid) > 0)
					if (PROC50)
						Deck.push_back(Src);
			}
			// repair
			if (aid == ACTIVATION_REPAIR)
			{
				effect = Src.GetAbility(aid) * FusionMultiplier;
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					GetTargets(Structures,faction,targets);
					if (targets.size())
					{
						PPCIV::iterator vi = targets.begin();
						while (vi != targets.end())
						{
							if (vi->first->GetHealth() == vi->first->GetMaxHealth())
								vi = targets.erase(vi);
							else vi++;
						}
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
						{
							destindex = UCHAR(rand() % targets.size());
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
						{
							vi->first->Heal(effect,QuestEffectId);
							if (!IsMimiced)
								Src.fsHealed += effect;
							else
								Mimicer->fsHealed += effect;
						}
					}
					targets.clear();
				}
			}
			// shock
			if (aid == ACTIVATION_SHOCK)
			{
				effect = Src.GetAbility(aid) * FusionMultiplier;
				if (effect > 0)
				{
					Src.fsDmgDealt += Dest.Commander.SufferDmg(QuestEffectId,effect);
					DamageToCommander += effect;
					FullDamageToCommander += effect;
				}
			}
			// siege
			if (aid == ACTIVATION_SIEGE)
			{
				effect = Src.GetAbility(aid) * FusionMultiplier;
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					if (chaos > 0)
						GetTargets(Structures,faction,targets);
					else	
						GetTargets(Dest.Structures,faction,targets);
					if (targets.size())
					{
						if (Src.GetTargetCount(aid) != TARGETSCOUNT_ALL)
						{
							destindex = UCHAR(rand() % targets.size());
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
					
							if ((vi->first->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && (PROC50) && (!chaos))
							{
								// evaded
								vi->first->fsAvoided += effect;
								Dest.SkillProcs[DEFENSIVE_EVADE]++;
							}
							else
							{
								UCHAR overkill = 0;
								UCHAR sdmg = vi->first->SufferDmg(QuestEffectId,effect,0,0,0,&overkill);
								Dest.CheckDeathEvents(*vi->first,*this);
								if (!IsMimiced)
								{
									Src.fsDmgDealt += sdmg;
									Src.fsOverkill += overkill;
								}
								else
								{
									Mimicer->fsDmgDealt += sdmg;
									Mimicer->fsOverkill += overkill;
								}
							}
					}
					targets.clear();
				}
			}
			// split
			if (aid == ACTIVATION_SPLIT)
			{
				effect = Src.GetAbility(ACTIVATION_SPLIT);
				if ((effect > 0) && (!IsMimiced))
				{
					//Units.push_back(PlayedCard(Src.GetOriginalCard()));
					// vector can be reallocated after push back, so I have to update Src
					// Src = Units[Position]; this doesn't work because I use reference to an object
					// workaround:
					bSplit = true;
				}
			}
			// strike
			if (aid == ACTIVATION_STRIKE)
			{
				effect = Src.GetAbility(aid) * FusionMultiplier;
				if (effect > 0) {
                    // mimiced strike?
					if (IsMimiced) {
						faction = FACTION_NONE;
					}else {
						faction = Src.GetTargetFaction(aid);
                    }
                    // chaosed strike?
					if (chaos > 0) {
						GetTargets(Units,faction,targets);
					} else {
						GetTargets(Dest.Units,faction,targets);
                    }
                    // do we have a target at all
					if (targets.size() > 0)	{
                        // is it not strike all?
						if (Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) {
                            // pick a random target
							destindex = UCHAR(rand() % targets.size());
							if (!chaos) {
								destindex = Intercept(targets, destindex, Dest);
                            }
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
                        // Targets should never be empty at this point
                        assert(!targets.empty());
						if (!targets.empty()) {
							if ((!IsMimiced) || bIsSelfMimic) {
								SkillProcs[aid]++;
							} else {
								Dest.SkillProcs[aid]++;
                            }
						}
                        // do for all targets
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++) {
							if ((vi->first->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && (PROC50) && (!chaos))
							{
								// evaded
								vi->first->fsAvoided += effect;
								Dest.SkillProcs[DEFENSIVE_EVADE]++;
							}
							else
							{
								if (bConsoleOutput)
								{
									Src.PrintDesc();
									printf(" strike ");
									vi->first->PrintDesc();
									printf(" for %d\n",effect);
								}
								UCHAR overkill = 0;
								UCHAR sdmg = vi->first->StrikeDmg(QuestEffectId,effect,&overkill);
								Dest.CheckDeathEvents(*vi->first,*this);
								if (!IsMimiced)
								{
									Src.fsDmgDealt += sdmg;
									Src.fsOverkill += overkill;
								}
								else
								{
									Mimicer->fsDmgDealt += sdmg;
									Mimicer->fsOverkill += overkill;
								}
								if (Src.IsAlive() && (EffectType != EVENT_DIED))
									if (vi->first->GetAbility(DEFENSIVE_PAYBACK) && (Src.GetType() == TYPE_ASSAULT) && PROC50 && (!chaos))  // payback
									{
										UCHAR overkill = 0;
										vi->first->fsDmgDealt += Src.StrikeDmg(QuestEffectId,effect,&overkill);
										CheckDeathEvents(Src,Dest);
										vi->first->fsOverkill += overkill;
										Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
									}
							}			
                        }
                    } else {
                        // no target to strike
                        LOG(this->logger,abilityFailNoTarget(EffectType,aid,Src,IsMimiced,(chaos>0),faction,effect));
                    }
				}
			}
			// summon
            if (aid == ACTIVATION_SUMMON) {
                effect = Src.GetAbility(ACTIVATION_SUMMON);
                if (effect > 0)	{
                    Card const * const summonedCard = &pCDB[effect];
                    if(summonedCard->GetType() == TYPE_ASSAULT) {
                        Units.push_back(summonedCard);
                        LOG(this->logger,abilitySummon(EffectType,Src,Units.back()));
                        Units.back().SetCardSkillProcBuffer(SkillProcs);
                        ApplyEffects(QuestEffectId,EVENT_PLAYED,Units.back(),-1,Dest);
                    } else if (summonedCard->GetType() == TYPE_STRUCTURE) {
                        Structures.push_back(summonedCard);
                        LOG(this->logger,abilitySummon(EffectType,Src,Structures.back()));
                        Structures.back().SetCardSkillProcBuffer(SkillProcs);
                        ApplyEffects(QuestEffectId,EVENT_PLAYED,Structures.back(),-1,Dest);
                    } else {
                        throw logic_error("Summoned something that is neither assault unit nor structure");
                    }
                }
            }


			// weaken
			// i've played like 20 times mission 66 just farming and noticed that AI,
			// when played beholder one of the first cards kept weakening my unit aligned
			// to beholder every fucking turn, so I died of fear almost every time enemy
			// played beholder 1-2nd card, I just couldn't kill it, Is he that smart?
			// I played rush cards so he had plenty cards to pick from to weaken
			// I fucking hate random ...
			// no, that not true, AI is dumb
			if (aid == ACTIVATION_WEAKEN)
			{
				effect = Src.GetAbility(aid) * FusionMultiplier;
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					if (chaos > 0)
						GetTargets(Units,faction,targets);
					else
						GetTargets(Dest.Units,faction,targets);
					if (targets.size())
					{
						PPCIV::iterator vi = targets.begin();
						while (vi != targets.end())
						{
							if ((vi->first->GetWait() == 0) && 
								(vi->first->GetAttack() >= 1) && // at least 1 Attack OR it is WEAKEN ALL, that can lower ur attack down to -100500
								(!vi->first->GetEffect(ACTIVATION_JAM)) && // neither Jammed
								(!vi->first->GetEffect(ACTIVATION_FREEZE)) && // neither Frozen
								(!vi->first->GetEffect(DMGDEPENDANT_IMMOBILIZE)) &&   // nor Immobilized
								((!vi->first->GetPlayed()) || (!chaos)) // if it is chaosed - only target cards that didn't play yet
								)
								vi++; // skip
							else
								vi = targets.erase(vi);
						}
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
						{
							destindex = UCHAR(rand() % targets.size());
							if (!chaos)
								destindex = Intercept(targets, destindex, Dest);
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
							if ((vi->first->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && (PROC50) && (!chaos))
							{
								// evaded
								Dest.SkillProcs[DEFENSIVE_EVADE]++;
							}
							else
							{
								if (bConsoleOutput)
								{
									Src.PrintDesc();
									printf(" weaken ");
									vi->first->PrintDesc();
									printf(" for %d\n",effect);
								}
								UCHAR we = vi->first->Weaken(effect);
								if (!IsMimiced)
									Src.fsSpecial += we;
								else
									Mimicer->fsSpecial += we;
								if (Src.IsAlive() && (EffectType != EVENT_DIED))
									if (vi->first->GetAbility(DEFENSIVE_PAYBACK) && (Src.GetType() == TYPE_ASSAULT) && (Src.GetAttack() > 0) && PROC50 && (!chaos))  // payback
									{
										vi->first->fsSpecial += Src.Weaken(effect);
										Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
									}
							}
					}
				}
			}
			// rush - not sure yet whether this skill is activation or not
			// can it be mimiced? it only presents on structures and commanders atm
			// it shouldn't be tributable
			if ((aid == ACTIVATION_RUSH) || ((QuestEffectId == QEFFECT_TIME_SURGE) && (!aindex)))
			{
				effect = Src.GetAbility(aid);
				if (aid != ACTIVATION_RUSH)
					effect = 1;
				if (effect > 0)
				{
					faction = Src.GetTargetFaction(aid);
					GetTargets(Units,faction,targets);
					if (targets.size())
					{
						PPCIV::iterator vi = targets.begin();
						while (vi != targets.end())
						{
							if (!vi->first->GetWait())
								vi = targets.erase(vi);
							else vi++;
						}
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
						{
							destindex = UCHAR(rand() % targets.size());
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
							SkillProcs[aid]++;
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
						{
							if (bConsoleOutput)
							{
								Src.PrintDesc();
								printf(" rush ");
								vi->first->PrintDesc();
								printf(" for %d\n",effect);
							}
							vi->first->Rush(effect);		
							Src.fsSpecial += effect;
						}
					}
					targets.clear();
				}
			}
			// Chaos		
			if (aid == ACTIVATION_CHAOS)
			{
				effect = Src.GetAbility(aid);
				if (effect > 0)
				{
					if (IsMimiced)
						faction = FACTION_NONE;
					else
						faction = Src.GetTargetFaction(aid);
					if (chaos > 0)
						GetTargets(Units,faction,targets);
					else
						GetTargets(Dest.Units,faction,targets);
					if (targets.size())
					{
						PPCIV::iterator vi = targets.begin();
						while (vi != targets.end())
						{
							if ((vi->first->GetWait() == 0) && 
								(!vi->first->GetEffect(ACTIVATION_JAM)) && // not Jammed
								(!vi->first->GetEffect(ACTIVATION_FREEZE)) && // not Frozen
								(!vi->first->GetEffect(ACTIVATION_CHAOS))   // not Chaosed
								)
								vi++; // skip
							else
								vi = targets.erase(vi);
						}
						if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
						{
							destindex = UCHAR(rand() % targets.size());
							if (!chaos)
								destindex = Intercept(targets, destindex, Dest);
							tmp = targets[destindex];
							targets.clear();
							targets.push_back(tmp);
						}
						if (!targets.empty())
						{
							if ((!IsMimiced) || bIsSelfMimic)
								SkillProcs[aid]++;
							else
								Dest.SkillProcs[aid]++;
						}
						for (PPCIV::iterator vi = targets.begin();vi != targets.end();vi++)
							if ((vi->first->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && (PROC50) && (!chaos))
							{
								// evaded
								Dest.SkillProcs[DEFENSIVE_EVADE]++;
							}
							else
							{
								if (bConsoleOutput)
								{
									Src.PrintDesc();
									printf(" chaos ");
									vi->first->PrintDesc();
									printf("\n");
								}
								vi->first->SetEffect(ACTIVATION_CHAOS,effect);
								UCHAR we = effect;
								if (!IsMimiced)
									Src.fsSpecial += we;
								else
									Mimicer->fsSpecial += we;
								if (Src.IsAlive() && (EffectType != EVENT_DIED))
									if (vi->first->GetAbility(DEFENSIVE_PAYBACK) && (Src.GetType() == TYPE_ASSAULT) && (Src.GetAttack() > 0) && PROC50 && (!chaos))  // payback
									{
										Src.SetEffect(ACTIVATION_CHAOS,effect);
										vi->first->fsSpecial += effect;
										Dest.SkillProcs[DEFENSIVE_PAYBACK]++;
									}
							}
					}
				}
			}
		}
		// split, finally, can't do it inside of the loop because it corrupts pointer to Src since vector can be moved
		if (bSplit)
		{
			Units.push_back(Src.GetOriginalCard());
			Units.back().SetCardSkillProcBuffer(SkillProcs);
			ApplyEffects(QuestEffectId,EVENT_PLAYED,Units.back(),-1,Dest);
		}
	}
	void SweepFancyStats(PlayedCard &pc)
	{
		if (!CSIndex) return;
		if (!CSResult) return;
		CSResult[CSIndex[pc.GetId()]].FSRecordCount++;
		CSResult[CSIndex[pc.GetId()]].FSAvoided += pc.fsAvoided;
		CSResult[CSIndex[pc.GetId()]].FSDamage += pc.fsDmgDealt;
		CSResult[CSIndex[pc.GetId()]].FSMitigated += pc.fsDmgMitigated;
		CSResult[CSIndex[pc.GetId()]].FSHealing += pc.fsHealed;
		CSResult[CSIndex[pc.GetId()]].FSSpecial += pc.fsSpecial;
		CSResult[CSIndex[pc.GetId()]].FSOverkill += pc.fsOverkill;
		CSResult[CSIndex[pc.GetId()]].FSDeaths += pc.fsDeaths;
	}
	void SweepFancyStatsRemaining()
	{
		if (!CSIndex) return;
		if (!CSResult) return;
		SweepFancyStats(Commander);
		for (LCARDS::iterator vi = Units.begin();vi != Units.end();vi++)
			SweepFancyStats(*vi);
		for (LCARDS::iterator vi = Structures.begin();vi != Structures.end();vi++)
			SweepFancyStats(*vi);
	}
	const Card *PickNextCard(bool bNormalPick = true)
	{
		// pick a random card
		LCARDS::iterator vi = Deck.begin();
		UCHAR indx = 0;
		if (vi != Deck.end()) // gay ass STL updates !!!
		{
			if (!bOrderMatters)
			{
				// standard random pick
				indx = UCHAR(rand() % Deck.size());
			}
			else
			{
				// pick that involves 'hand' and priorities
				// fill hand
				UCHAR handsize = 3;
				if (Deck.size() <= handsize)
				{
					Hand.clear();
					for (UCHAR i=0;i<Deck.size();i++)
						Hand.insert(i);
				}
				else
				{
					do
					{
						indx = UCHAR(rand() % Deck.size());
						// we need to pick first card of a same type, instead of picking this card
						for (UCHAR i=0;i<Deck.size();i++)
							if ((this->getUnitAt(indx).GetId() == this->getUnitAt(i).GetId()) && (Hand.find(indx) == Hand.end()) && (Hand.find(i) == Hand.end()))
							{
								indx = i;
								break;
							}
						if (Hand.find(indx) == Hand.end())
						{
							Hand.insert(indx);
							//printf("add to hand %d\n",indx);
						}
					}
					while (Hand.size() < handsize);
				}
				indx = (*Hand.begin()); // pick first in set
				//printf("pick %d\n",indx);
				MSID tHand;
				for (MSID::iterator si = Hand.begin(); si != Hand.end(); si++)
					if (si != Hand.begin()) // this one is picked
						tHand.insert((*si) - 1); // offset after pick
				Hand.clear();
				for (MSID::iterator si = tHand.begin(); si != tHand.end(); si++)
					Hand.insert(*si);

				/*for (MSID::iterator si = Hand.begin(); si != Hand.end(); si++)
				{
					printf("%d ",*si);
				}
				printf(" -> %d\n",indx);*/
			}
		}
		while(vi != Deck.end())
		{
			if (!indx)
			{
                Card const * const c = vi->GetOriginalCard();
				if (bNormalPick)
				{
					if (bConsoleOutput)
					{
						Commander.PrintDesc();
						printf(" picks ");
						vi->PrintDesc();
						printf("\n");
					}
					if (bDelayFirstCard)
					{
						vi->IncWait();
						bDelayFirstCard = false;
					}
					if (vi->GetType() == TYPE_ASSAULT)
					{
                        PlayedCard newAssaultCard(c);
						Units.push_back(newAssaultCard);
						Units.back().SetCardSkillProcBuffer(SkillProcs);
					}
					if (vi->GetType() == TYPE_STRUCTURE)
					{
						Structures.push_back(PlayedCard(c));
						Structures.back().SetCardSkillProcBuffer(SkillProcs);
					}
					if (vi->GetType() == TYPE_ACTION)
					{
						Actions.push_back(PlayedCard(c));
						Actions.back().SetCardSkillProcBuffer(SkillProcs);
					}
					for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
						if (!CardPicks[i])
						{
							CardPicks[i] = vi->GetId();
							break;
						}
					vi = Deck.erase(vi);
				}
				return c;
			}
			vi++;
			indx--;
		}
		return 0; // no cards for u
	}
	void CheckDeathEvents(PlayedCard &src, ActiveDeck &Def)
	{
		if (src.OnDeathEvent())
			ApplyEffects(QuestEffectId,EVENT_DIED,src,-1,Def);
	}
	void AttackDeck(ActiveDeck &Def, bool bSkipCardPicks = false)
	{
		// process poison
        for (LCARDS::iterator iter=Units.begin(); iter != Units.end(); iter++) {
            iter->ResetShield(); // according to wiki, shield doesn't affect poison, it wears off before poison procs I believe
            iter->ProcessPoison(QuestEffectId);
        }
		//  Moraku: If �Heal on Death� triggers from poison damage, it will NOT be able to heal another unit dying from poison damage on the same turn. (All poison damage takes place before �On Death� skills trigger)
        for (LCARDS::iterator iter=Units.begin(); iter != Units.end(); iter++) {
			if (iter->OnDeathEvent())
				ApplyEffects(QuestEffectId,EVENT_DIED,*iter,-1,Def);
        }
		// Quest split mark
		if (QuestEffectId == QEFFECT_CLONE_PROJECT)
		{
			PPCIV GetTo;
			for (LCARDS::iterator vi = Units.begin();vi != Units.end();vi++)
			{
				if ((vi->IsAlive()) && 
					(vi->GetWait() == 0) && 
					(!vi->GetEffect(ACTIVATION_JAM)) && // Jammed
					(!vi->GetEffect(ACTIVATION_FREEZE)) && // Frozen
					(!vi->GetEffect(DMGDEPENDANT_IMMOBILIZE)))
					GetTo.push_back(PPCARDINDEX(&(*vi),0));
			}
			if (!GetTo.empty())
			{
				UCHAR rc = UCHAR(rand() % GetTo.size());
				GetTo[rc].first->SetQuestSplit(true);
			}
		}

		if (!bSkipCardPicks)
		{
			const Card *c = PickNextCard();
			if (c)
			{
				if (c->GetType() == TYPE_ACTION)
					ApplyEffects(QuestEffectId,EVENT_PLAYED,Actions.front(),-1,Def);
				else
				if (c->GetType() == TYPE_STRUCTURE)
					ApplyEffects(QuestEffectId,EVENT_PLAYED,Structures.back(),-1,Def);
				else
				if (c->GetType() == TYPE_ASSAULT)
				{
					ApplyEffects(QuestEffectId,EVENT_PLAYED,Units.back(),-1,Def);
					// add quest statuses for decay
					if (QuestEffectId == QEFFECT_DECAY)
					{
						Units.back().SetEffect(DMGDEPENDANT_POISON,1);
						Units.back().SetEffect(DMGDEPENDANT_DISEASE,ABILITY_ENABLED);
					}
				}
			}
		}

		PlayedCard Empty;
		UCHAR iFusionCount = 0;
        for (LCARDS::const_iterator iter = Structures.begin(); iter != Structures.end(); iter++) {
            if (iter->GetAbility(SPECIAL_FUSION) > 0) {
                iFusionCount++;
            }
        }
		// action cards
        for (LCARDS::iterator iter = Actions.begin(); iter != Actions.end(); iter++) {
            // apply actions somehow ...
            ApplyEffects(QuestEffectId,EVENT_EMPTY,*iter,-1,Def);
        }
		for (LCARDS::iterator vi = Actions.begin();vi != Actions.end();vi++) {
			SweepFancyStats(*vi);
        }
		Actions.clear();
		// commander card
		// ok lets work out Infuse:
		// infuse - dont know how this works :(
		// ok so afaik it changes one random card from either of your or enemy deck into bloodthirsty
		// faction plus it changes heal and rally skills faction, if there were any, into bloodthirsty
		// i believe it can't be mimiced and paybacked(can assume since it's commander skill) and 
		// it can be evaded, according to forums
		// "his own units that have evade wont ever seem to evade.
		// (every time ive seen the collossus infuse and as far as i can see� he has no other non bt with evade.)"
		// so, I assume, evade works for us, but doesn't work for his cards
		// the bad thing about infuse is that we need faction as an attribute of card, we can't pull it out of
		// library, I need to add PlayedCard.Faction, instead of using Card.Faction
		// added
		if (Commander.IsDefined() && Commander.GetAbility(ACTIVATION_INFUSE) > 0)
		{
			// pick a card
			PPCIV targets;
			targets.reserve(DEFAULT_DECK_RESERVE_SIZE);
			GetTargets(Def.Units,FACTION_BLOODTHIRSTY,targets,true);
			UCHAR defcount = (UCHAR)targets.size();
			GetTargets(Units,FACTION_BLOODTHIRSTY,targets,true);
			if (!targets.empty())
			{
				UCHAR i = UCHAR(rand() % targets.size());
				i = Intercept(targets, i, Def); // we don't know anything about Infuse being interceptable :( I assume, it is
				PlayedCard *t = targets[i].first;
				if ((i < defcount) && (t->GetAbility(DEFENSIVE_EVADE) || (QuestEffectId == QEFFECT_QUICKSILVER)) && PROC50) // we check evade only on our cards, enemy cards don't seem to actually evade infuse since it's rather helpful to them then harmful
				{
					// evaded infuse
					//printf("Evaded\n");
				}
				else
					t->Infuse(FACTION_BLOODTHIRSTY);
			}
		}
		// apply actions same way 
		if (Commander.IsDefined()) {
			ApplyEffects(QuestEffectId,EVENT_EMPTY,Commander,-1,Def);
        }
		// structure cards
        { UCHAR i = 0;
        for (LCARDS::iterator iter = Structures.begin(); iter != Structures.end(); iter++,i++) {
			// apply actions somehow ...
			if (iter->BeginTurn()) {
				ApplyEffects(QuestEffectId,EVENT_EMPTY,*iter,-1,Def,false,(iFusionCount >= 3),0,i);
            }
			iter->EndTurn();
		}}
		// assault cards
		{ UCHAR i = 0;
        for (LCARDS::iterator iter = Units.begin(); iter != Units.end(); iter++,i++) {
			if (iter->BeginTurn()) {
				ApplyEffects(QuestEffectId,EVENT_EMPTY,*iter,i,Def);
				if ((!iter->GetEffect(DMGDEPENDANT_IMMOBILIZE)) && (!iter->GetEffect(ACTIVATION_JAM)) && (!iter->GetEffect(ACTIVATION_FREEZE))) // tis funny but I need to check Jam for second time in case it was just paybacked
				{
					if (iter->IsAlive() && iter->GetAttack() > 0 && Def.Commander.IsAlive()) { // can't attack with dead unit ;) also if attack = 0 then dont attack at all
						Attack(i,Def);
                    }
				}
			}
			iter->EndTurn();
		}}
		// refresh commander
		if (Commander.IsDefined() && Commander.GetAbility(DEFENSIVE_REFRESH)) // Bench told refresh procs at the end of player's turn
			Commander.Refresh(QuestEffectId);
		// clear dead units here yours and enemy
		if (!Units.empty())
		{
			LCARDS::iterator vi = Units.begin();
			while (vi != Units.end())
				if (!vi->IsAlive())
				{
					for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
						if (!CardDeaths[i])
						{
							CardDeaths[i] = vi->GetId();
							break;
						}
					SweepFancyStats(*vi);
					vi = Units.erase(vi);
				}
				else
				{
					vi->ResetPlayedFlag();
					vi->ClearEnfeeble(); // this is important for chaosed skills
					if ((!vi->IsDiseased()) && (vi->GetAbility(DEFENSIVE_REFRESH))) // Bench told refresh procs at the end of player's turn
						vi->Refresh(QuestEffectId);
					vi->RemoveDebuffs(); // post-turn
					vi++;
				}
		}
		if (!Structures.empty())
		{
			LCARDS::iterator vi = Structures.begin();
			while (vi != Structures.end())
				if (!vi->IsAlive())
				{
					for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
						if (!CardDeaths[i])
						{
							CardDeaths[i] = vi->GetId();
							break;
						}
					SweepFancyStats(*vi);
					vi = Structures.erase(vi);
				}
				else
				{
					if ((!vi->IsDiseased()) && (vi->GetAbility(DEFENSIVE_REFRESH)) && (QuestEffectId != QEFFECT_IMPENETRABLE)) // Bench told refresh procs at the end of player's turn
						vi->Refresh(QuestEffectId);
					vi++;
				}
		}
		//
		if (!Def.Units.empty())
		{
			LCARDS::iterator vi = Def.Units.begin();
			while (vi != Def.Units.end())
				if (!vi->IsAlive())
				{
					for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
						if (!Def.CardDeaths[i])
						{
							Def.CardDeaths[i] = vi->GetId();
							break;
						}
					Def.SweepFancyStats(*vi);
					vi = Def.Units.erase(vi);
				}
				else
				{
					vi->ClearEnfeeble(); // this is important for chaosed skills
					vi++;
				}
		}
		if (!Def.Structures.empty())
		{
			LCARDS::iterator vi = Def.Structures.begin();
			while (vi != Def.Structures.end())
				if (!vi->IsAlive())
				{
					for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
						if (!Def.CardDeaths[i])
						{
							Def.CardDeaths[i] = vi->GetId();
							break;
						}
					Def.SweepFancyStats(*vi);
					vi = Def.Structures.erase(vi);
				}
				else
					vi++;
		}
		// check if delete record from vector via iterator and then browse forward REALLY WORKS????
		// shift cards
	}
	void PrintShort()
	{
		std::cout << Commander.GetName() << " [";
        bool first = true;
		for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++) {
			if (first) {
                first = false;
            } else {
				std::cout << ",";
            }
			std::cout << iter->GetName();
		}
		std::cout << "]\n";
	}
	string GetDeck() const
	{
		if (Deck.empty())
			return string();
		string s;
		char buffer[10];
		if (Commander.IsDefined())
		{
			_itoa_s(Commander.GetId(),buffer,10);
			s.append(buffer);
		}
		for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++) {
			if (!s.empty())
				s.append(",");
			_itoa_s(iter->GetId(),buffer,10);
			s.append(buffer);
		}
		return s;
	}
	string GetHash64(bool bCardPicks = false) const
	{
#define HASH_SAVES_ORDER	1
		if (Deck.empty() && ((!bCardPicks) || (!CardPicks[0])))
			return string();
#if HASH_SAVES_ORDER
		typedef vector<UINT> MSID; 
#else
		typedef multiset<UINT> MSID; // I <3 sets, they keep stuff sorted ;)
#endif
		MSID ids;
        if (!bCardPicks) {
            for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++) {
#if HASH_SAVES_ORDER
                ids.push_back(iter->GetId());
#else
                ids.insert(iter->GetId());
#endif
            }
		}
		else
			for (UCHAR i=0;(i<DEFAULT_DECK_RESERVE_SIZE) && (CardPicks[i]);i++)
				ids.push_back(CardPicks[i]); // multiset is disallowed!
		string s;
		UINT tmp = 0, t;
		unsigned short lastid = 0, cnt = 1;
		if (Commander.IsDefined())
		{
			tmp = ID2BASE64(Commander.GetId());
			s.append((char*)&tmp);
			//printf("1: %s -commander\n",(char*)&tmp);
		}
		MSID::iterator si = ids.begin();
		do
		{
			// we can actually use Id range 4000-4095 (CARD_MAX_ID - 0xFFF) for special codes,
			// adding RLE here
			tmp = ID2BASE64(*si);
			si++;
			if ((lastid != tmp) || (si == ids.end()))
			{
				if (lastid)
				{
					t = lastid;
					if (cnt == 2)
					{
						s.append((char*)&t);
						s.append((char*)&t);
						//printf("4: %s -dupe\n",(char*)&t);
						//printf("4: %s -dupe\n",(char*)&t);
						
					}
					else
						if (cnt > 2)
						{
							s.append((char*)&t);
							//printf("3: %s -value\n",(char*)&t);
							t = ID2BASE64(CARD_MAX_ID + cnt); // special code, RLE count
							s.append((char*)&t); 
							//printf("3: %s -rle\n",(char*)&t);
						}
						else
						{
							s.append((char*)&t);
							//printf("5: %s\n",(char*)&t);
						}
					cnt = 1;
				}
				if (si == ids.end())
				{
					s.append((char*)&tmp);
					//printf("2: %s\n",(char*)&tmp);
				}
				lastid = tmp;
			}
			else
				cnt++;  // RLE, count IDs
		}
		while (si != ids.end());
		return s;
	}
protected:
	void GetTargets(LCARDS &From, UCHAR TargetFaction, PPCIV &GetTo, bool bForInfuse = false)
	{
		if (!bForInfuse)
			GetTo.clear();
		UCHAR pos = 0;
		for (LCARDS::iterator vi = From.begin();vi != From.end();vi++)
		{
			if ((vi->IsAlive()) && (((vi->GetFaction() == TargetFaction) && (!bForInfuse)) || (TargetFaction == FACTION_NONE) || ((vi->GetFaction() != TargetFaction) && (bForInfuse))))
				GetTo.push_back(PPCARDINDEX(&(*vi),pos));
			pos++;
		}
	}
};

    /**
     * Hit the commander.
     */
    bool PlayedCard::HitCommander(UINT QuestEffectId
                                 , const UCHAR Dmg
                                 , PlayedCard &Src
                                 , ActiveDeck & ownDeck
                                 , ActiveDeck & otherDeck
                                 , bool bCanBeCountered
                                 , UCHAR *overkill
                                 , VLOG *log
                                 , LOG_RECORD *lr
                                 )
	{
        LCARDS & Structures(ownDeck.Structures);
		_ASSERT(GetType() == TYPE_COMMANDER); // double check for debug

        // 0 dmg is pointless and indicates an error
        if(Dmg <= 0) {
            throw std::invalid_argument("Zero 0 damage in PlayedCard::HitCommander");
        }

		// find a wall to break it ;)
		UCHAR index = 0;
		for (LCARDS::iterator vi = Structures.begin();vi!=Structures.end();vi++)
		{
			if (vi->GetAbility(DEFENSIVE_WALL) && vi->IsAlive())
			{
				if (QuestEffectId != QEFFECT_IMPENETRABLE)
				{
					vi->CardSkillProc(DEFENSIVE_WALL);
					if (lr)
					{
						lr->Target.CardID = index;
						lr->Target.RowID = TYPE_STRUCTURE;
					}



					// walls can counter and regenerate
					vi->SufferDmg(QuestEffectId,Dmg,0,0,0,overkill);

                    // probably here wall's "on attacked" skills
                    ownDeck.ApplyEffects(QuestEffectId,EVENT_ATTACKED,*vi,index,otherDeck);

					if (vi->GetAbility(DEFENSIVE_COUNTER) && bCanBeCountered) // counter, dmg from crush can't be countered
					{
						vi->CardSkillProc(DEFENSIVE_COUNTER);
						EFFECT_ARGUMENT cdmg = vi->GetAbility(DEFENSIVE_COUNTER) + Src.GetEffect(ACTIVATION_ENFEEBLE);
						vi->fsDmgDealt += cdmg;
						UCHAR loverkill = 0;
						if (lr && log)
							log->push_back(LOG_RECORD(lr->Target,lr->Src,DEFENSIVE_COUNTER,cdmg));
						Src.SufferDmg(QuestEffectId,cdmg,0,0,0,&loverkill); // counter dmg is enhanced by enfeeble
						vi->fsOverkill += loverkill;
					}
				}
				return false;
			}
			index++;
		}

        // Commander was attacked, trigger event.
        ownDeck.ApplyEffects(QuestEffectId,EVENT_ATTACKED,*this,0,otherDeck);

		// no walls found then hit commander
		// ugly - counter procs before commander takes dmg, but whatever
		if (GetAbility(DEFENSIVE_COUNTER) && bCanBeCountered) // commander can counter aswell
		{
			CardSkillProc(DEFENSIVE_COUNTER);
			UCHAR loverkill = 0;
			EFFECT_ARGUMENT cdmg = GetAbility(DEFENSIVE_COUNTER) + Src.GetEffect(ACTIVATION_ENFEEBLE);
			if (lr && log)
				log->push_back(LOG_RECORD(lr->Target,lr->Src,DEFENSIVE_COUNTER,cdmg));
			Src.SufferDmg(QuestEffectId,cdmg,0,0,0,&loverkill); // counter dmg is enhanced by enfeeble
			fsOverkill += loverkill;
		}
		return (SufferDmg(QuestEffectId,Dmg,0,0,0,overkill) > 0);
	}
