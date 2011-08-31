#include <vector>
#include <set>

#define CARD_NAME_MAX_LENGTH	50 // must sync it with CARD_NAME_MAX_LENGTH in interface
#define FILENAME_MAX_LENGTH		50 //
#define CARD_ABILITIES_MAX		70 // must sync it with CARD_ABILITIES_MAX in interface

#define DEFAULT_DECK_RESERVE_SIZE	15 // up to 20?

#define CARD_MAX_ID				4000 // size of storage array
#define MISSION_MAX_ID			150  // size of storage array
#define RAID_MAX_ID				20  // size of storage array

typedef	unsigned char UCHAR;
typedef	unsigned int UINT;

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

char FACTIONS[5][CARD_NAME_MAX_LENGTH] = {0,"Imperial","Raider","Bloodthirsty","Xeno"};

//#define TARGETSCOUNT_NONE		0				
#define TARGETSCOUNT_ONE		0				
#define TARGETSCOUNT_ALL		10	

#define TYPE_NONE				0
#define TYPE_COMMANDER			1
#define TYPE_ASSAULT			2
#define TYPE_STRUCTURE			3
#define TYPE_ACTION				4

#define ABILITY_ENABLED			1  // just a helper, no use

#define ACTIVATION_ENFEEBLE		11
#define ACTIVATION_HEAL			12
#define ACTIVATION_INFUSE		13	// this currently works only for Commander	
#define ACTIVATION_JAM			14
#define ACTIVATION_MIMIC		15
#define ACTIVATION_RALLY		16
#define ACTIVATION_RECHARGE		17
#define ACTIVATION_REPAIR		18
#define ACTIVATION_SHOCK		19
#define ACTIVATION_SIEGE		20
#define ACTIVATION_SPLIT		21
#define ACTIVATION_STRIKE		22
#define ACTIVATION_WEAKEN		23

#define DEFENSIVE_ARMORED		31
#define DEFENSIVE_COUNTER		32
#define DEFENSIVE_EVADE			33
#define DEFENSIVE_FLYING		34
#define DEFENSIVE_PAYBACK		35
#define DEFENSIVE_REGENERATE	36
#define DEFENSIVE_WALL			37

#define COMBAT_ANTIAIR			41
#define COMBAT_FEAR				42
#define COMBAT_FLURRY			43
#define COMBAT_PIERCE			44
#define COMBAT_SWIPE			45
#define COMBAT_VALOR			46

#define DMGDEPENDANT_CRUSH		51
#define DMGDEPENDANT_IMMOBILIZE	52
#define DMGDEPENDANT_LEECH		53
#define DMGDEPENDANT_POISON		54
#define DMGDEPENDANT_SIPHON		55 

#define SPECIAL_BACKFIRE		61  // Destroyed - When this is destroyed, deal damage to own Commander.

// SKILLS THAT ARE NOT DEFINED AND NOT WORKING:
#define SPECIAL_FUSION			62	// in this sim only works for ACTIVATION skills of STRUCTURES
#define SPECIAL_BERSERK			63
#define SPECIAL_PROTECTION		64
#define SPECIAL_AUGMENT			65
#define SPECIAL_MIST			66 // this skill doesn't change anything in autoplay // max number refers to CARD_ABILITIES_MAX

#define UNDEFINED_NAME			"UNDEFINED"

using namespace std;

bool bConsoleOutput = false; // this is global just for convinience, should be DEFINE instead, to cleanup the code


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
	UCHAR Effects[CARD_ABILITIES_MAX];
	UCHAR TargetCounts[CARD_ABILITIES_MAX];
	UCHAR TargetFactions[CARD_ABILITIES_MAX]; // reserved negative values for faction
protected:
	void CopyName(const char *name)
	{
		if (name)
		{
			UCHAR len = strlen(name);
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
			UCHAR len = strlen(pic);
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
		memset(Effects,0,CARD_ABILITIES_MAX);
		memset(TargetCounts,0,CARD_ABILITIES_MAX);
		memset(TargetFactions,0,CARD_ABILITIES_MAX);
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
		memset(Effects,0,CARD_ABILITIES_MAX);
		memset(TargetCounts,0,CARD_ABILITIES_MAX);
		memset(TargetFactions,0,CARD_ABILITIES_MAX);
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
		memcpy(Effects,card.Effects,CARD_ABILITIES_MAX);
		memcpy(TargetCounts,card.TargetCounts,CARD_ABILITIES_MAX);
		memcpy(TargetFactions,card.TargetFactions,CARD_ABILITIES_MAX);
	}
	void AddAbility(const UCHAR id, const UCHAR effect, const UCHAR targetcount, const UCHAR targetfaction)
	{
		Effects[id] = effect;
		TargetCounts[id] = targetcount;
		TargetFactions[id] = targetfaction;
	}
	void AddAbility(const UCHAR id, const UCHAR targetcount, const UCHAR targetfaction)
	{
		Effects[id] = ABILITY_ENABLED;
		TargetCounts[id] = targetcount;
		TargetFactions[id] = targetfaction;
	}
	void AddAbility(const UCHAR id, const UCHAR effect)
	{
		Effects[id] = effect;
	}
	void AddAbility(const UCHAR id)
	{
		Effects[id] = ABILITY_ENABLED;
	}
	void Destroy() { Id = 0; }
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
	const UCHAR GetRarity() const { return Rarity; }
	const UCHAR GetFaction() const { return Faction; }
	const UCHAR GetAbility(const UCHAR id) const { return Effects[id]; }
	const UCHAR GetTargetCount(const UCHAR id) const { return TargetCounts[id]; }
	const UCHAR GetTargetFaction(const UCHAR id) const { return TargetFactions[id]; }
	const char *GetName() const { return Name; }
	const char *GetPicture() const { return Picture; }
};

class PlayedCard
{
typedef vector<PlayedCard> VCARDS;
typedef vector<PlayedCard*> PVCARDS;
private:
	// constant
	const Card *OriginalCard;
	// temporary
	UCHAR Attack;
	UCHAR Health;
	UCHAR Wait;
	UCHAR Faction; // this is for Infuse
	UCHAR Effects[CARD_ABILITIES_MAX];
	bool bPlayed;
public:
	void DecWait() { if (Wait) Wait--; }
	const char *GetName() const { return OriginalCard->GetName(); }
	void PrintDesc()
	{
		if (IsDefined())
		{
			if (Attack && Wait)
				printf("%s [%d/%d|%d]",OriginalCard->GetName(),Attack,Health,Wait);
			else
				if (Wait)
					printf("%s |%d",OriginalCard->GetName(),Attack,Health,Wait);
				else
					printf("%s [%d]",OriginalCard->GetName(),Attack,Health,Wait);
			if (Effects[ACTIVATION_JAM])
				printf(" Jammed");
			if (Effects[DMGDEPENDANT_IMMOBILIZE])
				printf(" Immobilized");
		}
	}
	const bool BeginTurn()
	{
		Effects[ACTIVATION_ENFEEBLE] = 0;
		if (Effects[DMGDEPENDANT_POISON])
			SufferDmg(Effects[DMGDEPENDANT_POISON]);
		return (Health && (!Effects[ACTIVATION_JAM]) && (!Wait));
	}
	void EndTurn()
	{
		Attack = OriginalCard->GetAttack();
		Effects[ACTIVATION_JAM] = 0;
		Effects[DMGDEPENDANT_IMMOBILIZE] = 0;
		Played(); // for rally
		if (Wait > 0)
			Wait--;
	}
	void Infuse(const UCHAR setfaction)
	{
		//OriginalCard.Infuse(setfaction);
		Faction = setfaction;
	}
	const UCHAR SufferDmg(const UCHAR Dmg)
	{
		_ASSERT(OriginalCard);
// Regeneration happens before the additional strikes from Flurry.
// Regenerating does not prevent Crush damage
		if (Dmg >= Health)
		{
			if ((OriginalCard->GetAbility(DEFENSIVE_REGENERATE))&&(PROC50))
				Health = OriginalCard->GetAbility(DEFENSIVE_REGENERATE);
			else
				Health = 0;
			// crush damage will be dealt even if the defending unit Regenerates
			return Health;// note that CRUSH & BACKFIRE are processed elsewhere
		}
		else
			Health -= Dmg;
		return Dmg;
	}
	bool HitCommander(const UCHAR Dmg, PlayedCard &Src, VCARDS &Structures, bool bCanBeCountered = true)
	{
		_ASSERT(GetType() == TYPE_COMMANDER); // double check for debug
		_ASSERT(Dmg); // 0 dmg is pointless and indicates an error
		// find a wall to break it ;)
		for (VCARDS::iterator vi = Structures.begin();vi!=Structures.end();vi++)
			if (vi->GetAbility(DEFENSIVE_WALL) && vi->IsAlive())
			{
				// walls can counter and regenerate
				vi->SufferDmg(Dmg); // regenerate
				if (vi->GetAbility(DEFENSIVE_COUNTER) && bCanBeCountered) // counter, dmg from crush can't be countered
					Src.SufferDmg(vi->GetAbility(DEFENSIVE_COUNTER) + Src.GetEffect(ACTIVATION_ENFEEBLE)); // counter dmg is enhanced by enfeeble
				return false;
			}
		// no walls found then hit commander
		// ugly - counter procs before commander takes dmg, but whatever
		if (GetAbility(DEFENSIVE_COUNTER) && bCanBeCountered) // commander can counter aswell
			Src.SufferDmg(GetAbility(DEFENSIVE_COUNTER) + Src.GetEffect(ACTIVATION_ENFEEBLE)); // counter dmg is enhanced by enfeeble
		return (SufferDmg(Dmg) > 0);
	}
	UCHAR StrikeDmg(const UCHAR Dmg) // returns dealt dmg
	{
		_ASSERT(Dmg); // 0 dmg is pointless and indicates an error
		//printf("%s %d <- %d\n",GetName(),GetHealth(),Dmg);
		return SufferDmg(Dmg + Effects[ACTIVATION_ENFEEBLE]);
	}
	const bool IsAlive() const
	{
		return ((OriginalCard) &&/* THIS IS CRAP! */(Health != 0)); //(Attack||Health||Wait);
	}
	const bool IsDefined() const
	{
		return (OriginalCard && (Attack||Health||Wait||(GetType() == TYPE_COMMANDER)||(GetType() == TYPE_ACTION)));
	}
	const UCHAR GetRarity() const
	{
		return OriginalCard->GetRarity();
	}
	PlayedCard& operator=(const Card *card)
	{
		_ASSERT(card);
		OriginalCard = card;
		Attack = card->GetAttack();
		Health = card->GetHealth();
		Wait = card->GetWait();
		Faction = card->GetFaction();
		bPlayed = false;
		memset(Effects,0,CARD_ABILITIES_MAX);
		return *this;
	}
	PlayedCard(const Card *card) 
	{
		_ASSERT(card);
		OriginalCard = card;
		Attack = card->GetAttack();
		Health = card->GetHealth();
		Wait = card->GetWait();
		Faction = card->GetFaction();
		bPlayed = false;
		memset(Effects,0,CARD_ABILITIES_MAX);
	}
	const UINT GetId() const { return OriginalCard->GetId(); }
	const UCHAR GetAttack() const { return Attack; }
	const UCHAR GetHealth() const { return Health; }
	const UCHAR GetMaxHealth() const { return OriginalCard->GetHealth(); }
	const UCHAR GetFaction() const { return Faction; }
	const UCHAR GetWait() const { return Wait; }
	const UCHAR GetType() const	
	{
		// this is crap ! - must remove and check the code
		if (!OriginalCard)
			return TYPE_NONE;
		return OriginalCard->GetType();	
	}
	const UCHAR GetEffect(const UCHAR id) const { return Effects[id]; }
	const UCHAR GetAbility(const UCHAR id) const
	{
		// this is crap ! - must remove and check the code
		if (!OriginalCard)
			return 0;
		return OriginalCard->GetAbility(id); 
	}
	const UCHAR GetTargetCount(const UCHAR id) const { return OriginalCard->GetTargetCount(id); }
	const UCHAR GetTargetFaction(const UCHAR id) const { return OriginalCard->GetTargetFaction(id); }
	const bool GetPlayed() const { return bPlayed; }
	void Played() { bPlayed = true; }
	void ResetPlayedFlag() { bPlayed = false; }
	void SetHealth(const UCHAR health) { Health = health; }
	void SetEffect(const UCHAR id, const UCHAR value) { Effects[id] = value; }
	void Rally(const UCHAR amount) { Attack += amount; }
	void Weaken(const UCHAR amount) { if (amount > Attack) Attack = 0; else Attack -= amount; }
	void Heal(const UCHAR amount)
	{
		if (Health + amount >  OriginalCard->GetHealth())
			Health =  OriginalCard->GetHealth();
		else
			Health += amount;
	}
	const Card *GetOriginalCard() const { return OriginalCard; }
//protected:
	PlayedCard() 
	{
		Attack = 0;
		Health = 0;
		Wait = 0;
		Faction = 0;
		bPlayed = false;
		OriginalCard = 0;
		memset(Effects,0,CARD_ABILITIES_MAX);
	}
};

typedef vector<PlayedCard> VCARDS;
typedef vector<PlayedCard*> PVCARDS;

class ActiveDeck
{
private:
public:
	PlayedCard Commander;
	VCARDS Deck;
	VCARDS Units;
	VCARDS Structures;
	VCARDS Actions;
private:
	void Reserve()
	{
		Deck.reserve(DEFAULT_DECK_RESERVE_SIZE);
		Units.reserve(DEFAULT_DECK_RESERVE_SIZE);
		Structures.reserve(DEFAULT_DECK_RESERVE_SIZE);
		Actions.reserve(DEFAULT_DECK_RESERVE_SIZE);
	}
	void Attack(UCHAR index, ActiveDeck &Def)
	{
#define SRC	Units[index]
// does anyone know if VALOR procs on commander? imagine combo of valor+flurry or valor+fear
// ok let's assume it does
#define VALOR_HITS_COMMANDER	true
		//printf("%s %d\n",SRC.GetName(),SRC.GetHealth());
		_ASSERT(SRC.IsDefined() && SRC.IsAlive()); // baby, don't hurt me, no more
		UCHAR iflurry = (SRC.GetAbility(COMBAT_FLURRY) && PROC50) ? SRC.GetAbility(COMBAT_FLURRY) : 1; // coding like a boss :) don't like this style
		if ((index >= (UCHAR)Def.Units.size()) || (!Def.Units[index].IsAlive()) || (SRC.GetAbility(COMBAT_FEAR)))
		{
			// Deal DMG To Commander BUT STILL PROC50 FLURRY and PROBABLY VALOR
			UCHAR valor = (VALOR_HITS_COMMANDER && SRC.GetAbility(COMBAT_VALOR) && (Units.size() < Def.Units.size())) ? SRC.GetAbility(COMBAT_VALOR) : 0;
			for (UCHAR i=0;i<iflurry;i++)
				Def.Commander.HitCommander(SRC.GetAttack()+valor,SRC,Def.Structures);
			return; // and thats it!!!
		}
		else
		{
			// oh noes! that wasn't all
			PlayedCard *targets[3];
			// flurry + swipe ...
			UCHAR swipe = (SRC.GetAbility(COMBAT_SWIPE)) ? 3 : 1;
			if (swipe > 1)
			{
				if ((index > 0) && (Def.Units[index-1].IsAlive()))
					targets[0] = &Def.Units[index-1];
				else
					targets[0] = 0;
				targets[1] = &Def.Units[index];
				_ASSERT(targets[1]); // this is aligned to SRC and must be present
				if ((index+1 < (UCHAR)Def.Units.size()) && (Def.Units[index+1].IsAlive()))
					targets[2] = &Def.Units[index+1];
				else
					targets[2] = 0;
			}
			else
				targets[0] = &Def.Units[index];
			// flurry
			for (UCHAR iatk=0;iatk<iflurry;iatk++)
			{
				// swipe
				for (UCHAR s=0;s<swipe;s++)
				{
					if (((!s) || (s == 2)) && ((!targets[s]) || (!targets[s]->IsAlive()))) // empty slot to the left or right, swipe doesnt proc, if swipe == 1 then targets[0] can't be null
						continue;
					// if target dies during flurry and slot(s == 1) is aligned to SRC, we deal dmg to commander
					if ((!targets[s]->IsAlive()) && ((swipe == 1) || (s == 1)))
					{
						UCHAR valor = (VALOR_HITS_COMMANDER && SRC.GetAbility(COMBAT_VALOR) && (Units.size() < Def.Units.size())) ? SRC.GetAbility(COMBAT_VALOR) : 0;
						Def.Commander.HitCommander(SRC.GetAttack()+valor,SRC,Def.Structures);
						// might want to add here check:
						// if (!Def.Commander.IsAlive()) return;
						continue;
					}
					_ASSERT(targets[s]->IsAlive()); // must be alive here
					// actual attack
					// must check valor before every attack
					UCHAR valor = (SRC.GetAbility(COMBAT_VALOR) && (Units.size() < Def.Units.size())) ? SRC.GetAbility(COMBAT_VALOR) : 0;
					// attacking flyer
					UCHAR antiair = SRC.GetAbility(COMBAT_ANTIAIR);
					if (targets[s]->GetAbility(DEFENSIVE_FLYING))
					{
						if ((!antiair) && PROC50) // missed
							continue;
					}
					else antiair = 0; // has no effect if target is not flying
					// enfeeble is taken into account before armor
					UCHAR enfeeble = targets[s]->GetEffect(ACTIVATION_ENFEEBLE);
					// now armor & pierce
					UCHAR dmg = SRC.GetAttack() + valor + antiair + enfeeble;
					UCHAR armor = targets[s]->GetAbility(DEFENSIVE_ARMORED);
					if (armor) 
					{
						UCHAR pierce = SRC.GetAbility(COMBAT_PIERCE);
						if (pierce >= armor)
							armor = 0;
						else
							armor -= pierce;
						// substract armor from dmg
						if (armor >= dmg)
							dmg = 0;
						else
							dmg -= armor; 
					}
					// now we actually deal dmg
					//printf("%s %d = %d => %s %d\n",SRC.GetName(),SRC.GetHealth(),dmg,targets[s]->GetName(),targets[s]->GetHealth());
					if (dmg)
						dmg = targets[s]->SufferDmg(dmg);
					// and now dmg dependant effects
					if (!targets[s]->IsAlive()) // target just died
					{
						// afaik it ignores walls
						if (targets[s]->GetAbility(SPECIAL_BACKFIRE))
							Def.Commander.SufferDmg(targets[s]->GetAbility(SPECIAL_BACKFIRE));
						// crush
						if (SRC.GetAbility(DMGDEPENDANT_CRUSH))
							Def.Commander.HitCommander(SRC.GetAbility(DMGDEPENDANT_CRUSH),SRC,Def.Structures,false);
					}
					// counter
					if (targets[s]->GetAbility(DEFENSIVE_COUNTER))
						SRC.SufferDmg(targets[s]->GetAbility(DEFENSIVE_COUNTER) + SRC.GetEffect(ACTIVATION_ENFEEBLE)); // counter dmg is enhanced by enfeeble
					// if target is dead, we dont need to process this effects
					if (targets[s]->IsAlive())
					{
						// immobilize
						if (SRC.GetAbility(DMGDEPENDANT_IMMOBILIZE) && PROC50)
							targets[s]->SetEffect(DMGDEPENDANT_IMMOBILIZE,SRC.GetAbility(DMGDEPENDANT_IMMOBILIZE));
						// poison
						if (SRC.GetAbility(DMGDEPENDANT_POISON))
							if (targets[s]->GetEffect(DMGDEPENDANT_POISON) < SRC.GetAbility(DMGDEPENDANT_POISON)) // overflow
								targets[s]->SetEffect(DMGDEPENDANT_POISON,SRC.GetAbility(DMGDEPENDANT_POISON));
					}
					// leech
					if (SRC.IsAlive() && SRC.GetAbility(DMGDEPENDANT_LEECH))
					{
						UCHAR leech = (SRC.GetAbility(DMGDEPENDANT_LEECH) < dmg) ? SRC.GetAbility(DMGDEPENDANT_LEECH) : dmg;
						if (leech)
							SRC.Heal(leech);
					}
					// siphon
					if (SRC.GetAbility(DMGDEPENDANT_SIPHON))
					{
						UCHAR siphon = (SRC.GetAbility(DMGDEPENDANT_SIPHON) < dmg) ? SRC.GetAbility(DMGDEPENDANT_SIPHON) : dmg;
						if (siphon)
							Commander.Heal(siphon);
					}

					if (!SRC.IsAlive()) // died from counter? during swipe
						break;
				}
				if (!SRC.IsAlive()) // died from counter? during flurry
					break;
			}
		}
	}
public:
	ActiveDeck() {}
	~ActiveDeck() { Deck.clear(); Units.clear(); Structures.clear(); Actions.clear(); }
public:
	// please note, contructors don't clean up storages, must do it manually and beforehand, even copy constructor
	ActiveDeck(const char *HashBase64, const Card *pCDB)
	{
		_ASSERT(pCDB);
		_ASSERT(HashBase64);
		unsigned short tid = 0, lastid = 0;
		size_t len = strlen(HashBase64);
		_ASSERT(!(len & 1)); // bytes should go in pairs
		len = len >> 1; // div 2
		Deck.reserve(DEFAULT_DECK_RESERVE_SIZE);
		for (UCHAR i = 0; i < len; i++)
		{
			tid = BASE64ID((HashBase64[i << 1] << 8) + HashBase64[(i << 1) + 1]);
			if (!i)
			{
				_ASSERT(tid < CARD_MAX_ID);
				_ASSERT((tid >= 1000) && (tid < 2000)); // commander Id boundaries
				Commander = PlayedCard(&pCDB[tid]);
			}
			else
			{
				_ASSERT(i || (tid < CARD_MAX_ID)); // commander card can't be encoded with RLE
				if (tid < CARD_MAX_ID)
				{
					Deck.push_back(&pCDB[tid]);
					lastid = tid;
				}
				else
				{
					for (UINT k = CARD_MAX_ID+1; k < tid; k++) // decode RLE, +1 because we already added one card
						Deck.push_back(&pCDB[lastid]);
				}					
			}
		}
	}
	ActiveDeck(const Card *Cmd) { Commander = PlayedCard(Cmd); Deck.reserve(DEFAULT_DECK_RESERVE_SIZE); };
	ActiveDeck(const ActiveDeck &D) // need copy constructor
	{
		Commander = D.Commander;
		Deck.reserve(D.Deck.size());
		for (UCHAR i=0;i<D.Deck.size();i++)
			Deck.push_back(D.Deck[i]);
		//for (VCARDS::iterator vi = D.Deck.begin();vi != D.Deck.end();vi++)
		//	Deck.push_back(*vi);
	}
	void Add(const Card *c)
	{
		Deck.push_back(c);
	}
	void ApplyEffects(PlayedCard &Src,ActiveDeck &Dest,bool IsMimiced=false,bool IsFusioned=false)
	{
		UCHAR destindex,aid,faction;
		PVCARDS targets;
		targets.reserve(DEFAULT_DECK_RESERVE_SIZE);
		PlayedCard *tmp;
		UCHAR effect;
		UCHAR FusionMultiplier = 1;
		if (IsFusioned)
			FusionMultiplier = 2;

		// enfeeble
		aid = ACTIVATION_ENFEEBLE;
		effect = Src.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = Src.GetTargetFaction(aid);
			GetTargets(Dest.Units,faction,targets);
			if (targets.size())
			{
				if (Src.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					destindex = rand() % targets.size();
					tmp = targets[destindex];
					targets.clear();
					targets.push_back(tmp);
				}
				for (PVCARDS::iterator vi = targets.begin();vi != targets.end();vi++)
					if (((*vi)->GetAbility(DEFENSIVE_EVADE)) && (PROC50))
					{
						// evaded
					}
					else
					{
						(*vi)->SetEffect(aid,(*vi)->GetEffect(aid) + effect);
						if ((*vi)->GetAbility(DEFENSIVE_PAYBACK) && (Src.GetType() == TYPE_ASSAULT) && PROC50)  // payback
							Src.SetEffect(aid,Src.GetEffect(aid) + effect);
						if (bConsoleOutput)
						{
							Src.PrintDesc();
							printf(" enfeeble ");
							(*vi)->PrintDesc();
							printf(" for %d\n",effect);
						}
					}
			}
			targets.clear();
		}
		// heal
		aid = ACTIVATION_HEAL;
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
				PVCARDS::iterator vi = targets.begin();
				while (vi != targets.end())
				{
					if ((*vi)->GetHealth() == (*vi)->GetMaxHealth())
						vi = targets.erase(vi);
					else vi++;
				}
				if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
				{
					destindex = rand() % targets.size();
					tmp = targets[destindex];
					targets.clear();
					targets.push_back(tmp);
				}
				for (vi = targets.begin();vi != targets.end();vi++)
				{
					if (bConsoleOutput)
					{
						Src.PrintDesc();
						printf(" heals ");
						(*vi)->PrintDesc();
						printf(" for %d\n",effect);
					}
					(*vi)->Heal(effect);
				}
			}
			targets.clear();
		}
		// infuse is processed on the upper level
		// ******
		// jam
		aid = ACTIVATION_JAM;
		effect = Src.GetAbility(aid);
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = Src.GetTargetFaction(aid);
			GetTargets(Dest.Units,faction,targets);
			if (targets.size())
			{
				PVCARDS::iterator vi = targets.begin();
				while (vi != targets.end())
				{
					if (((*vi)->GetEffect(aid)) || ((*vi)->GetWait()))
						vi = targets.erase(vi);
					else vi++;
				}
				if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
				{
					destindex = rand() % targets.size();
					tmp = targets[destindex];
					targets.clear();
					targets.push_back(tmp);
				}
				for (PVCARDS::iterator vi = targets.begin();vi != targets.end();vi++)
					if (PROC50)
						if (((*vi)->GetAbility(DEFENSIVE_EVADE)) && (PROC50))
						{
							// evaded
						}
						else
						{
							(*vi)->SetEffect(aid,effect);
/*  ?  */					if ((*vi)->GetAbility(DEFENSIVE_PAYBACK) && (Src.GetType() == TYPE_ASSAULT) && PROC50)  // payback is it 1/2 or 1/4 chance to return jam with payback????
								Src.SetEffect(aid,effect);
							if (bConsoleOutput)
							{
								Src.PrintDesc();
								printf(" jam ");
								(*vi)->PrintDesc();
								printf("\n");
							}
						}
			}
			targets.clear();
		}
		// mimic - could be tricky
		aid = ACTIVATION_MIMIC;
		effect = Src.GetAbility(aid);
		if ((effect > 0) && (!IsMimiced))
		{
			if (IsMimiced) // mimic can't be mimiced ;) it's just sad copypaste, previous line prevents this being mimiced
				faction = FACTION_NONE;
			else
				faction = Src.GetTargetFaction(aid);
			GetTargets(Dest.Units,faction,targets);
			if (targets.size())
			{
				if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL))
				{
					destindex = rand() % targets.size();
					tmp = targets[destindex];
					targets.clear();
					targets.push_back(tmp);
				}
				for (PVCARDS::iterator vi = targets.begin();vi != targets.end();vi++)
					if (((*vi)->GetAbility(DEFENSIVE_EVADE)) && (PROC50))
					{
						// evaded
					}
					else
					{
						if (bConsoleOutput)
						{
							Src.PrintDesc();
							printf(" mimic ");
							(*vi)->PrintDesc();
							printf("\n");
						}
						ApplyEffects(*(*vi),Dest,true);	
					}
			}
			targets.clear();
		}
		// rally
		// does rally prefer units with 0 attack? ???? or its completely random
		// afaik it's completely random
		aid = ACTIVATION_RALLY;
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
				PVCARDS::iterator vi = targets.begin();
				while (vi != targets.end())
				{
					if (((*vi)->GetWait()) || (*vi)->GetPlayed())
						vi = targets.erase(vi);
					else vi++;
				}
				if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
				{
					destindex = rand() % targets.size();
					tmp = targets[destindex];
					targets.clear();
					targets.push_back(tmp);
				}
				for (PVCARDS::iterator vi = targets.begin();vi != targets.end();vi++)
				{
					if (bConsoleOutput)
					{
						Src.PrintDesc();
						printf(" rally ");
						(*vi)->PrintDesc();
						printf(" for %d\n",effect);
					}
					(*vi)->Rally(effect);
				}
			}
			targets.clear();
		}
		// recharge -  only action cards
		if (Src.GetAbility(ACTIVATION_RECHARGE) > 0)
			if (PROC50)
				Deck.push_back(Src);
		// repair
		aid = ACTIVATION_REPAIR;
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
				PVCARDS::iterator vi = targets.begin();
				while (vi != targets.end())
				{
					if ((*vi)->GetHealth() == (*vi)->GetMaxHealth())
						vi = targets.erase(vi);
					else vi++;
				}
				if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
				{
					destindex = rand() % targets.size();
					tmp = targets[destindex];
					targets.clear();
					targets.push_back(tmp);
				}
				for (PVCARDS::iterator vi = targets.begin();vi != targets.end();vi++)
					(*vi)->Heal(effect);
			}
			targets.clear();
		}
		// shock
		effect = Src.GetAbility(ACTIVATION_SHOCK) * FusionMultiplier;
		if (effect > 0)
			Dest.Commander.SufferDmg(effect);
		// siege
		aid = ACTIVATION_SIEGE;
		effect = Src.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = Src.GetTargetFaction(aid);
			GetTargets(Dest.Structures,faction,targets);
			if (targets.size())
			{
				if (Src.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					destindex = rand() % targets.size();
					tmp = targets[destindex];
					targets.clear();
					targets.push_back(tmp);
				}
				for (PVCARDS::iterator vi = targets.begin();vi != targets.end();vi++)
					
					if (((*vi)->GetAbility(DEFENSIVE_EVADE)) && (PROC50))
					{
						// evaded
					}
					else
						(*vi)->SufferDmg(effect);
			}
			targets.clear();
		}
		// split
		effect = Src.GetAbility(ACTIVATION_SPLIT);
		if ((effect > 0) && (!IsMimiced))
			Units.push_back(PlayedCard(Src.GetOriginalCard()));
		// strike - Only targets active Assault cards on play with at least 1 Attack that are neither Jammed nor Immobilized
		aid = ACTIVATION_STRIKE;
		effect = Src.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = Src.GetTargetFaction(aid);
			GetTargets(Dest.Units,faction,targets);
			if (targets.size())
			{
				if (Src.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					destindex = rand() % targets.size();
					tmp = targets[destindex];
					targets.clear();
					targets.push_back(tmp);
				}
				for (PVCARDS::iterator vi = targets.begin();vi != targets.end();vi++)
					if (((*vi)->GetAbility(DEFENSIVE_EVADE)) && (PROC50))
					{
						// evaded
					}
					else
					{
						if (bConsoleOutput)
						{
							Src.PrintDesc();
							printf(" strike ");
							(*vi)->PrintDesc();
							printf(" for %d\n",effect);
						}
						(*vi)->StrikeDmg(effect);
						if ((*vi)->GetAbility(DEFENSIVE_PAYBACK) && (Src.GetType() == TYPE_ASSAULT) && PROC50)  // payback
							Src.StrikeDmg(effect);
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
		aid = ACTIVATION_WEAKEN;
		effect = Src.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = Src.GetTargetFaction(aid);
			GetTargets(Dest.Units,faction,targets);
			if (targets.size())
			{
				PVCARDS::iterator vi = targets.begin();
				while (vi != targets.end())
				{
					if (((*vi)->GetWait() <= 1) &&
						((*vi)->GetAttack() >= 1) && // at least 1 Attack
						(!(*vi)->GetEffect(ACTIVATION_JAM)) && // neither Jammed
						(!(*vi)->GetEffect(DMGDEPENDANT_IMMOBILIZE))    // nor Immobilized
						)
						vi++; // skip
					else
						vi = targets.erase(vi);
				}
				if ((Src.GetTargetCount(aid) != TARGETSCOUNT_ALL) && (!targets.empty()))
				{
					destindex = rand() % targets.size();
					tmp = targets[destindex];
					targets.clear();
					targets.push_back(tmp);
				}
				for (PVCARDS::iterator vi = targets.begin();vi != targets.end();vi++)
					if (((*vi)->GetAbility(DEFENSIVE_EVADE)) && (PROC50))
					{
						// evaded
					}
					else
					{
						if (bConsoleOutput)
						{
							Src.PrintDesc();
							printf(" weaken ");
							(*vi)->PrintDesc();
							printf(" for %d\n",effect);
						}
						(*vi)->Weaken(effect);
						if ((*vi)->GetAbility(DEFENSIVE_PAYBACK) && (Src.GetType() == TYPE_ASSAULT) && (Src.GetAttack() > 0) && PROC50)  // payback
							Src.Weaken(effect);
					}
			}
		}
	}
	void AttackDeck(ActiveDeck &Def)
	{
		// pick a random card
		VCARDS::iterator vi = Deck.begin();
		UCHAR indx = 0;
		if (vi != Deck.end()) // gay ass STL updates !!!
			indx = rand() % Deck.size();
		while(vi != Deck.end())
		{
			if (!indx)
			{
				if (bConsoleOutput)
				{
					Commander.PrintDesc();
					printf(" picks ");
					vi->PrintDesc();
					printf("\n");
				}
				if (vi->GetType() == TYPE_ASSAULT)
					Units.push_back(*vi);
				if (vi->GetType() == TYPE_STRUCTURE)
					Structures.push_back(*vi);
				if (vi->GetType() == TYPE_ACTION)
					Actions.push_back(*vi);
				vi = Deck.erase(vi);
				break;
			}
			vi++;
			indx--;
		}

		PlayedCard Empty;
		UCHAR iFusionCount = 0;
		for (UCHAR i=0;i<Structures.size();i++)
			if (Structures[i].GetAbility(SPECIAL_FUSION) > 0)
				iFusionCount++;
		// action cards
		for (UCHAR i=0;i<Actions.size();i++)
		{
			// apply actions somehow ...
			ApplyEffects(Actions[i],Def);
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
		// (every time ive seen the collossus infuse and as far as i can see… he has no other non bt with evade.)"
		// so, I assume, evade works for us, but doesn't work for his cards
		// the bad thing about infuse is that we need faction as an attribute of card, we can't pull it out of
		// library, I need to add PlayedCard.Faction, instead of using Card.Faction
		// added
		if (Commander.GetAbility(ACTIVATION_INFUSE) > 0)
		{
			// pick a card
			PVCARDS targets;
			targets.reserve(DEFAULT_DECK_RESERVE_SIZE);
			GetTargets(Def.Units,FACTION_BLOODTHIRSTY,targets,true);
			UCHAR defcount = targets.size();
			GetTargets(Units,FACTION_BLOODTHIRSTY,targets,true);
			if (!targets.empty())
			{
				UCHAR i = rand() % targets.size(); 
				PlayedCard *t = targets[i];
				if ((i < defcount) && (t->GetAbility(DEFENSIVE_EVADE)) && PROC50) // we check evade only on our cards, enemy cards don't seem to actually evade infuse since it's rather helpful to them then harmful
				{
					// evaded infuse
					//printf("Evaded\n");
				}
				else
					t->Infuse(FACTION_BLOODTHIRSTY);
			}
		}
		// apply actions same way 
		ApplyEffects(Commander,Def);
		// structure cards
		for (UCHAR i=0;i<Structures.size();i++)
		{
			// apply actions somehow ...
			if (Structures[i].BeginTurn())
				ApplyEffects(Structures[i],Def,false,(iFusionCount >= 3));
			Structures[i].EndTurn();
		}
		// assault cards
		for (UCHAR i=0;i<Units.size();i++)
		{
			if (Units[i].BeginTurn())
			{
				//if (!Units[i].GetEffect(ACTIVATION_JAM)) // jammed - checked in beginturn
				ApplyEffects(Units[i],Def);
				if ((!Units[i].GetEffect(DMGDEPENDANT_IMMOBILIZE)) /*&& (!Units[i].GetEffect(ACTIVATION_JAM))*/) 
				{
					if (Units[i].IsAlive() && Units[i].GetAttack()) // can't attack with dead unit ;) also if attack = 0 then dont attack at all
						Attack(i,Def);
				}
			}
			Units[i].EndTurn();
		}
		// clear dead units here yours and enemy
		vi = Units.begin();
		while (vi != Units.end())
			if (!vi->IsAlive())
				vi = Units.erase(vi);
			else
			{
				vi->ResetPlayedFlag();
				vi++;
			}
		vi = Structures.begin();
		while (vi != Structures.end())
			if (!vi->IsAlive())
				vi = Structures.erase(vi);
			else
				vi++;
		vi = Actions.begin();
		while (vi != Actions.end())
			if (!vi->IsAlive())
				vi = Actions.erase(vi);
			else
				vi++;
		//
		vi = Def.Units.begin();
		while (vi != Def.Units.end())
			if (!vi->IsAlive())
				vi = Def.Units.erase(vi);
			else
				vi++;
		vi = Def.Structures.begin();
		while (vi != Def.Structures.end())
			if (!vi->IsAlive())
				vi = Def.Structures.erase(vi);
			else
				vi++;
		vi = Def.Actions.begin();
		while (vi != Def.Actions.end())
			if (!vi->IsAlive())
				vi = Def.Actions.erase(vi);
			else
				vi++;
		// check if delete record from vector via iterator and then browse forward REALLY WORKS????
		// shift cards
	}
	void PrintShort()
	{
		printf("%s [",Commander.GetName());
		for (UCHAR i=0;i<Deck.size();i++)
		{
			if (i)
				printf(",");
			printf(Deck[i].GetName());
		}
		printf("]\n");
	}
	string GetHash64() const
	{
		typedef multiset<UINT> SID; // I <3 sets, they keep stuff sorted ;)
		SID ids;
		for (UCHAR i=0;i<Deck.size();i++)
			ids.insert(Deck[i].GetId());
		string s;
		UINT tmp = 0, t = 0;
		unsigned short lastid = 0, cnt = 1;
		if (Commander.IsDefined())
		{
			tmp = ID2BASE64(Commander.GetId());
			s.append((char*)&tmp);
			//printf("1: %s\n",(char*)&tmp);
		}
		SID::iterator si = ids.begin();
		do
		{
			// we can actually use Id range 4000-4095 (CARD_MAX_ID - 0xFFF) for special codes,
			// adding RLE here
			tmp = ID2BASE64(*si);
			si++;
			if (lastid == tmp)
			{
				// RLE, count IDs
				cnt++;
			}
			if ((lastid != tmp) || (si == ids.end()))
			{
				if (cnt == 2)
				{
					t = lastid;
					s.append((char*)&t);
					//printf("4: %s\n",(char*)&t);
				}
				else
					if (cnt > 2)
					{
						t = ID2BASE64(CARD_MAX_ID + cnt); // special code, RLE count
						s.append((char*)&t); 
						//printf("3: %s\n",(char*)&t);
						cnt = 1;
					}
			}
			if (lastid != tmp)
			{
				lastid = tmp;
				s.append((char*)&tmp);
				//printf("2: %s\n",(char*)&tmp);
			}
		}
		while (si != ids.end());
		return s;
	}
protected:
	void GetTargets(VCARDS &From, UCHAR TargetFaction, PVCARDS &GetTo, bool bForInfuse = false)
	{
		if (!bForInfuse)
			GetTo.clear();
		for (VCARDS::iterator vi = From.begin();vi != From.end();vi++)
			if ((vi->IsAlive()) && (((vi->GetFaction() == TargetFaction) && (!bForInfuse)) || (TargetFaction == FACTION_NONE) || ((vi->GetFaction() != TargetFaction) && (bForInfuse))))
				GetTo.push_back(&(*vi));
	}
};