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
// this module contains card database related classes - info, xml loader, card storage

#include <map>
#include <vector>
#include <string>
#include "pugixml\src\pugixml.hpp"
#include "pugixml\src\pugixml.cpp"

typedef map<string, UINT> MSUINT;
typedef pair<string, UINT> PAIRMSUINT;
typedef vector<string> VSTRINGS;
typedef map<string, UCHAR> MSKILLS;
typedef pair<string, UCHAR> PAIRMSKILLS;
typedef set <UINT>		SCID;
typedef map<UINT, UINT> MUUINT;
typedef pair<UINT, UINT> PAIRMUUINT;
#define TAG_ANY			-100500
#define TAG_BATCHEVAL	100500
#define TAG_CUSTOM		0
#define TAG_SOMERAID	-1000
#define TAG_SOMEMISSION	1000
struct DeckIndex
{
	string Name;
	int Tag;
	DeckIndex(string name)
	{
		Name = name;
		Tag = 0;
	}
	DeckIndex() {};
	DeckIndex(string name, int tag)
	{
		Name = name;
		Tag = tag;
	}
	~DeckIndex() {};
	const char *c_str() const
	{
		return Name.c_str();
	}
	bool operator<(const DeckIndex &DI) const
	{
		if (Name < DI.Name)
			return true;
		else
			if (Name > DI.Name)
				return false;
			else
				return Tag < DI.Tag;
	}
};
typedef map<DeckIndex, VSTRINGS> MDECKS;
typedef pair<DeckIndex, VSTRINGS> PAIRMDECKS;
struct CardSet
{
	char Name[CARD_NAME_MAX_LENGTH];
	char Icon[CARD_NAME_MAX_LENGTH];
	bool Visible;
	CardSet()
	{
		Name[0] = 0;
		Icon[0] = 0;
		Visible = false;
	}
	CardSet(const char *name, const char *icon, bool visible)
	{
		strcpy_s((char *)Name,CARD_NAME_MAX_LENGTH,name);
		strcpy_s((char *)Icon,CARD_NAME_MAX_LENGTH,icon);
		Visible = visible;
	}
};
typedef map<UINT, CardSet> MSETS;
typedef pair<UINT, CardSet> PAIRMSETS;
typedef vector<UINT> VID;
class MissionInfo
{
private:
	string Name;
	UINT Commander;
	VID Deck;
public:
	MissionInfo()
	{
		Commander = 0;
	}
	MissionInfo(const UINT commander, const char *name = 0)
	{
		Commander = commander;
		if (name)
			Name = string(name);
		Deck.reserve(DEFAULT_DECK_RESERVE_SIZE);
	}
	void Add(const UINT cardID)
	{
		Deck.push_back(cardID);
	}
	bool GetDeck(char *buffer, size_t buffersize)
	{
		char lbuff[10];
		for (UCHAR i=0;i<Deck.size();i++)
		{
			if (itoa(Deck[i],lbuff,10))
			{
				if (buffer[0] != 0)
					strcat_s(buffer,buffersize,",");
				strcat_s(buffer,buffersize,lbuff);
			}
		}
		return (buffer[0] != 0);
	}
	const UINT GetCommander() const { return Commander; }
	const char *GetName() const { return Name.c_str(); }
	const UINT GetCardCount() const { return (UINT)Deck.size(); }
	const UINT GetCardID(UINT Index) const { return Deck[Index]; }
	~MissionInfo()
	{
		Commander = 0;
		Deck.clear();
	}
	const bool IsValid() const { return (Commander != 0); }
};
// helpers
bool IsCardInDeck(const UINT Id, VCARDS &deck)
{
	if ((Id) && (!deck.empty()))
		for (UCHAR i = 0;i < deck.size();i++)
		{
			if (deck[i].GetId() == Id)
				return true;
		}
	return false;
}
void PickACard(Card *pCDB, VID &fromIDpool, VCARDS &topool)
{
	bool bLegendary = false;
	if (!fromIDpool.empty())
	{
		UCHAR indx = 0;
		for (VCARDS::iterator vi = topool.begin();vi != topool.end();vi++)
			if (vi->GetRarity() == RARITY_LEGENDARY)
				bLegendary = true;
		do
		{
			indx = UCHAR(rand() % fromIDpool.size());
		}
		while (((pCDB[fromIDpool[indx]].GetRarity() == RARITY_UNIQUE) && IsCardInDeck(fromIDpool[indx],topool))
			|| (bLegendary && (pCDB[fromIDpool[indx]].GetRarity() == RARITY_LEGENDARY)));   // unique check    

		topool.push_back(&pCDB[fromIDpool[indx]]);
		fromIDpool.erase(fromIDpool.begin()+indx);
	}
};
struct CardPool
{
#define DEFAULT_POOL_COUNT	5
#define DEFAULT_POOL_SIZE	10
	UCHAR Amount;
	VID Pool;
public:
	CardPool() {};
	CardPool(UCHAR amount)
	{
		Amount = amount;
		Pool.reserve(DEFAULT_POOL_SIZE);
	}
	CardPool(const CardPool &C) // copy constructor
	{
		Amount = C.Amount;
		Pool.clear();
		for (UCHAR i=0;i<C.Pool.size();i++)
			Pool.push_back(C.Pool[i]);
	}
	~CardPool() { Pool.clear(); };
	void GetPool(Card *pCDB, VCARDS &Deck) const
	{
		_ASSERT(Amount); // invalid pool
		_ASSERT(!Pool.empty()); // invalid pool
		// we must copy a pool into temporary
		VID tmpPool;
		tmpPool.reserve(Pool.size());
		for (UCHAR i=0;i<Pool.size();i++)
			tmpPool.push_back(Pool[i]);
		for (UCHAR i=0;i<Amount;i++)
			PickACard(pCDB,tmpPool,Deck);
		tmpPool.clear();
	}
};
typedef vector<CardPool> VCARDPOOL;
class RaidInfo
{
private:
	string Name;
	UINT Commander;
	VID AlwaysInclude;
	VCARDPOOL Pools;
public:
	RaidInfo() {}
	RaidInfo(UINT commander, const char *name)
	{
		Commander = commander;
		if (name)
			Name = string(name);
		AlwaysInclude.reserve(DEFAULT_DECK_RESERVE_SIZE);
		Pools.reserve(DEFAULT_POOL_COUNT);
	}
	RaidInfo(RaidInfo &RI)
	{
		Name = RI.Name;
		Commander = RI.Commander;
		AlwaysInclude.clear();
		for (UCHAR i=0;i<RI.AlwaysInclude.size();i++)
			AlwaysInclude.push_back(RI.AlwaysInclude[i]);
		for (UCHAR i=0;i<RI.Pools.size();i++)
			Pools.push_back(RI.Pools[i]);
	}
	void GetAlways(Card *pCDB, VCARDS &Deck) const
	{
		for (UCHAR i=0;i<AlwaysInclude.size();i++)
			if (pCDB[AlwaysInclude[i]].IsCard())
				Deck.push_back(&pCDB[AlwaysInclude[i]]);
	}
	void GetPools(Card *pCDB, VCARDS &Deck) const
	{
		for (UCHAR i=0;i<Pools.size();i++)
			Pools[i].GetPool(pCDB,Deck);
	}
	const UINT GetCommander() { return Commander; };
	const char *GetName() const { return Name.c_str(); }
	void AddAlways(const UINT cardID)
	{
		AlwaysInclude.push_back(cardID);
	}
	void AddPool(const CardPool &p)
	{
		Pools.push_back(p);
	}
	bool IsValid() { return (Commander != 0); }
	~RaidInfo()
	{ 
		AlwaysInclude.clear();
		Pools.clear();
	}
};
struct SKILL
{
	char SkillName[CARD_NAME_MAX_LENGTH];
	bool IsPassive;
	float CardValue;
	SKILL()
	{
		SkillName[0] = 0;
	}	
};
#define CARD_NAME_SYNTAX_EXCLUSION	"Kapak, the Deceiver"
char *FormatCardName(char *Name)
{
	UCHAR cut = 0;
	for (UCHAR i=0;i<250;i++)
		if (!Name[i]) break;
		else
		{
			if (Name[i] == ',')
				cut++;
			if (cut > 0)
				Name[i] = Name[i+cut];
		}
	return Name;
}
class CardDB
{	
	Card CDB[CARD_MAX_ID]; // this can cause stack overflow, but should work fastest
	MissionInfo MDB[MISSION_MAX_ID];
	RaidInfo RDB[RAID_MAX_ID];
	MSUINT MIIndex;
	MSUINT RIIndex;
	MSUINT Index;
	MDECKS DIndex;
	//char SKILLS[CARD_ABILITIES_MAX][CARD_NAME_MAX_LENGTH];
	MSKILLS SIndex;
public://protected:
	SKILL Skills[CARD_ABILITIES_MAX];
	MSETS SetIndex;
	CardDB() 
	{
		memset(CDB,0,CARD_MAX_ID * sizeof(Card));
		Initialize(); 
	};
private: // helper functions
	bool SaveIndex(const char *FileName, MDECKS &index)
	{
		FILE *f;
		if (!fopen_s(&f,FileName,"w"))
		{
			for (MDECKS::iterator mi = index.begin(); mi != index.end(); mi++)
			{
				fprintf_s(f,"%s:",mi->first.c_str());
				for (VSTRINGS::iterator vi = mi->second.begin(); vi != mi->second.end(); vi++)
				{
					if (vi != mi->second.begin())
						fprintf_s(f,",");
					fprintf_s(f,"%s",vi->c_str());
				}
				fprintf_s(f,"\n");
			}
			return (fclose(f) == 0);
		}
		return false;
	}
	UCHAR DetectTypeByID(UINT ID)
	{
		// Card ID < 1000 = Creature, Card ID < 2000 = Commander, Card ID < 3000 = Building, else Spell.
		if (ID < 1000)
			return TYPE_ASSAULT;
		if (ID < 2000)
			return TYPE_COMMANDER;
		if (ID < 3000)
			return TYPE_STRUCTURE;
		return TYPE_ACTION;
	}
	UCHAR RemapFaction(UCHAR XmlID)
	{
	//<unitType> <id>1</id> <name>Imperial</name> <image>CardImperialStyled</image> </unitType> -<unitType> 
	//<unitType> <id>3</id> <name>Bloodthirsty</name> <image>CardBloodthirstyStyle</image> </unitType>
	//<unitType> <id>4</id> <name>Xeno</name> <image>CardXenoStyle</image> </unitType> 
	//<unitType> <id>9</id> <name>Raider</name> <image>CardRaiderStyle</image> </unitType>
		if (XmlID == 1)
			return FACTION_IMPERIAL;
		if (XmlID == 3)
			return FACTION_BLOODTHIRSTY;
		if (XmlID == 4)
			return FACTION_XENO;
		if (XmlID == 8)
			return FACTION_RIGHTEOUS;
		if (XmlID == 9)
			return FACTION_RAIDER;
		return FACTION_NONE;
	}
	UCHAR RemapRarity(UCHAR XmlID, UCHAR UniqueFlag = 0)
	{
		if (XmlID == 1)
			return RARITY_COMMON;
		if (XmlID == 2)
			return RARITY_UNCOMMON;
		if (XmlID == 3)
		{
			if (UniqueFlag)
				return RARITY_UNIQUE;
			else
				return RARITY_RARE;
		}
		if (XmlID == 4)
		{
			if (UniqueFlag)
				return RARITY_LEGENDARY;
			else
				return RARITY_LEGENDARY; // that was only for Fenrir ??????
		}
		if (XmlID == 10)
			return RARITY_STORYCOMMANDER;
		// rarity UNKNOWN
		return RARITY_COMMON;
	}
public:
	const Card *GetPointer() const { return (const Card *)&CDB[0]; }
	bool LoadCardXML(const char *FileName, char *returnnewcards = 0, size_t MaxBufferSize = 0)
	{
		pugi::xml_document doc;
		if (returnnewcards)
			returnnewcards[0] = 0;
		if (!doc.load_file(FileName)) return false;

		size_t loaded = 0;
		size_t ss = 0;
		pugi::xml_node root = doc.child("root");
		for (pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
		{
			if (!_strcmpi(it->name(),"skillType"))
			{
				const char *id = it->child("id").child_value();
				//const char *name = it->child("name").child_value();
				//<cardValue passive='1' cost='2.5' />
				for (pugi::xml_node child = it->first_child(); child; child = child.next_sibling())
				{
					if (!_strcmpi(child.name(),"cardValue"))
					{
						MSKILLS::iterator si = SIndex.find(id);
						if (si == SIndex.end())
						{
							if (bConsoleOutput)
								printf("Skill \"%s\" not found in index!\n",id);
						}
						else
						{
							Skills[si->second].IsPassive = child.attribute("passive").as_bool();
							Skills[si->second].CardValue = child.attribute("cost").as_float();
							//printf("%s : %d %.1f\n",id,child.attribute("passive").as_bool(),child.attribute("cost").as_float()); 
						}
					}
				}
			}
			if (!_strcmpi(it->name(),"unit"))
			{
				char Name[250];
				strcpy(Name,it->child("name").child_value());
				FormatCardName(Name);
				UINT Id = atoi(it->child("id").child_value());
				const char *Pic = it->child("picture").child_value();
				UCHAR Attack = atoi(it->child("attack").child_value());
				UCHAR Health = atoi(it->child("health").child_value());				
				UCHAR Rarity = atoi(it->child("rarity").child_value());
				UCHAR Faction = RemapFaction(atoi(it->child("type").child_value()));
				UCHAR Wait = atoi(it->child("cost").child_value());
				UCHAR Unique = atoi(it->child("unique").child_value());
				UINT Set = atoi(it->child("set").child_value());
				/*if ((!Wait) && (it->child("attack").empty()) && (it->child("cost").empty()))
				{
					printf("Set : %d\n",Set);
					ss = Set;
				}*/
				//if (Id == 426)
				//{
				//	printf("Apollo: %d\n",Id);
				//}
				Card c(Id,Name,Pic,RemapRarity(Rarity,Unique),DetectTypeByID(Id)/*(Attack,Health,Wait,Faction,(!it->child("attack").empty()),(!it->child("cost").empty()))*/,Faction,Attack,Health,Wait,Set);
				//printf("%s: %d %d %d %d %d\n",Name,Attack,Health,Rarity,Faction,Wait);
				//<name>Duncan</name>
				//<attack>0</attack>
				//<health>10</health>
				//<rarity>2</rarity>
				//<skill x="1" id="rally" y="9"/>
				//<skill x="1" id="siege"/>
				//<skill x="2" id="heal" y="3" all="1"/> 
				//<skill x="1" id="repair" all="1"/>
				//<type>9</type>
				for (pugi::xml_node child = it->first_child(); child; child = child.next_sibling())
				{
					//printf("%s\n",child.name());
					//printf("  %s = %s\n",child.name(),child.child_value());
					if (!_strcmpi(child.name(),"skill"))
					{
						UCHAR Id = GetSkillID(child.attribute("id").value());
						_ASSERT(Id); // unknown skill
						if (!Id)
							continue;
						UCHAR Effect = child.attribute("x").as_uint();
						if (!Effect)
							Effect = ABILITY_ENABLED; // this value can't be 0, since it will disable the ability
						UCHAR TC = child.attribute("all").as_uint();
						if (!TC)
							TC = TARGETSCOUNT_ONE;
						else
							TC = TARGETSCOUNT_ALL;
						UCHAR TF = RemapFaction(child.attribute("y").as_uint());
						UCHAR skillevent = EVENT_EMPTY;
						if (child.attribute("died").as_uint() > 0)
							skillevent += EVENT_DIED;
						if (child.attribute("played").as_uint() > 0)
							skillevent += EVENT_PLAYED;
						c.AddAbility(Id,Effect,TC,TF,skillevent);
						//for (pugi::xml_attribute attr = child.first_attribute(); attr; attr = attr.next_attribute())
							//printf("    %s = %s\n",attr.name(),attr.value());
					}
				}
				//MSUINT::iterator mi = Index.find(c.GetName());
				//if (mi != Index.end())
				//	printf("Conflicted: %s : %d and %d\n",c.GetName(),mi->second,c.GetId());
				// so card should be treated as new if it's picture changed, so API would know it should download new pic
				bool isnew = ((!CDB[Id].IsCard()) || (stricmp(CDB[Id].GetPicture(),c.GetPicture())));
				bool ins = Insert(c);
				if (ins && returnnewcards && isnew)
				{
					if (returnnewcards[0])
						strcat_s(returnnewcards,MaxBufferSize,",");
					strcat_s(returnnewcards,MaxBufferSize,it->child("id").child_value());
				}
				loaded++;
			}
			else
				if (!_strcmpi(it->name(),"cardSet"))
				{
					UINT Id = atoi(it->child("id").child_value());
					const char *Name = it->child("name").child_value();
					const char *Icon = it->child("icon").child_value();
					UINT Visible = atoi(it->child("visible").child_value());
					SetIndex.insert(PAIRMSETS(Id,CardSet((char*)Name,(char*)Icon,(Visible != 0))));
				}
		}
		return (loaded > 0);
	}
	bool LoadMissionXML(const char *FileName)
	{
		pugi::xml_document doc;
		if (!doc.load_file(FileName)) return false;

		size_t loaded = 0;
		MIIndex.clear(); // clean index, we don't really need to clean array, it doesn't take too much space and cleaning it up will take time
		pugi::xml_node root = doc.child("root");
		for (pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
		{
			if (!_strcmpi(it->name(),"mission"))
			{
				const char *Name = it->child("name").child_value();
				UINT Id = atoi(it->child("id").child_value());
				UINT Commander = atoi(it->child("commander").child_value());

				_ASSERT(Id < MISSION_MAX_ID);
				if (Id >= MISSION_MAX_ID)
					continue;
				MDB[Id] = MissionInfo(Commander,Name);
				MIIndex.insert(PAIRMSUINT(Name,Id));

				pugi::xml_node deck = it->child("deck");
				for (pugi::xml_node_iterator di = deck.begin(); di != deck.end(); ++di)
				{
					_ASSERT(!_strcmpi(di->name(),"card")); // all siblings must be "card", but, i'd better check
					MDB[Id].Add(atoi(di->child_value()));
					//printf("%d : %s\n",Id,di->child_value());
				}
				loaded++;				
			}			
		}
		return (loaded > 0);
	}
	bool LoadRaidXML(const char *FileName)
	{
		pugi::xml_document doc;
		if (!doc.load_file(FileName)) return false;

		size_t loaded = 0;
		RIIndex.clear(); // clean index, we don't really need to clean array, it doesn't take too much space and cleaning it up will take time
		pugi::xml_node root = doc.child("root");
		for (pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
		{
			if (!_strcmpi(it->name(),"raid"))
			{
				const char *Name = it->child("name").child_value();
				UINT Id = atoi(it->child("id").child_value());
				UINT Commander = atoi(it->child("commander").child_value());

				_ASSERT(Id < RAID_MAX_ID);
				RDB[Id] = RaidInfo(Commander,Name);
				RIIndex.insert(PAIRMSUINT(Name,Id));
				loaded++;

				pugi::xml_node deck = it->child("deck");
				pugi::xml_node alwaysinclude = deck.child("always_include");
				for (pugi::xml_node_iterator di = alwaysinclude.begin(); di != alwaysinclude.end(); ++di)
				{
					_ASSERT(!_strcmpi(di->name(),"card")); // all siblings must be "card", but, i'd better check
					RDB[Id].AddAlways(atoi(di->child_value()));
				}
				for (pugi::xml_node_iterator di = deck.begin(); di != deck.end(); ++di)
					if (!strcmpi(di->name(),"card_pool"))
					{
						CardPool p(di->attribute("amount").as_int());
						for (pugi::xml_node_iterator ti = di->begin(); ti != di->end(); ++ti)
						{
							//_ASSERT(!_strcmpi(ti->name(),"card")); // all siblings must be "card", but, i'd better check
							// ok here can be mistakes, gotta seed them out
							if (!_strcmpi(ti->name(),"card"))
								p.Pool.push_back(atoi(ti->child_value()));
						}
						RDB[Id].AddPool(p);	
					}							
			}
			
		}
		return (loaded > 0);
	}
	bool RateCard(const UINT Id, double &OffenceValue, double &DefenceValue, const UCHAR iFormulaVersion = 0)
	{
		const Card *c = &CDB[Id];
		if (!c->IsCard()) // invalid
			return false;
		float fd = 0.0, fo = 0.0;
		for (UCHAR k=0;k<c->GetAbilitiesCount();k++)
		{
			UCHAR aid = c->GetAbilityInOrder(k);
			UCHAR cnt = c->GetTargetCount(aid);
			if (cnt < 1)
				cnt = 1;
			float fmod = ((float)cnt + 5) / 6;//(cnt + 1) / 2;
			if (c->GetTargetFaction(aid) != FACTION_NONE)
				fmod *= 0.75; // should be 0.75 or 0.8 methinks
			//ss += c->GetAbility(aid) * DB.Skills[aid].CardValue * fmod;
			//printf("[%d]: %d x %.1f x %.1f ~ %.1f | ",cnt,c->GetAbility(aid),DB.Skills[aid].CardValue,fmod,c->GetAbility(aid) * DB.Skills[aid].CardValue * fmod);
			//if (c->GetTargetFaction(aid) != FACTION_NONE)
			//	printf(" specific");
			//if (c->GetTargetFaction(aid) != FACTION_NONE)
			//	fmod /= 2;
			if (Skills[aid].IsPassive)
				fd += c->GetAbility(aid) * Skills[aid].CardValue;
			else
				fo += c->GetAbility(aid) * Skills[aid].CardValue * fmod;
			//f += c->GetAbility(aid) * DB.Skills[aid].CardValue;/* * fmod /** (1 + DB.Skills[aid].IsPassive) / 1*/;
			//printf("%d : %d x %.1f = %.1f | ",aid,c->GetAbility(aid),DB.Skills[aid].CardValue,c->GetAbility(aid) * DB.Skills[aid].CardValue);
		}
		//printf("%.1f + %.1f	%.1f	|",fd,fo,fo+fd+2.5*c->GetAttack()+1.5*(c->GetHealth()/*-1*/)-2.5*c->GetWait());
		//printf("	%.1f\n",fo / (c->GetWait() + 1)+fd+2.5*c->GetAttack() / (c->GetWait() + 0.5)+1.5*(c->GetHealth()/*-1*/));
		/*printf("	D %.1f	O- %.1f	O* %.1f\n",
			fd + 1.5*(c->GetHealth()),
			fo + 2.5*c->GetAttack() - 2.5*c->GetWait(),
			fo / (c->GetWait() + 0.5) + 2.5*c->GetAttack() / (c->GetWait() + 0.5)
			);*/
		if (!iFormulaVersion)
		{
			OffenceValue = (fo + 1.5*c->GetAttack()) * (10 - c->GetWait()) / 10;
			DefenceValue = fd + 1*(c->GetHealth());
		}
		else
		{
			OffenceValue = fo + 2.5*c->GetAttack() - 2.5*c->GetWait();
			DefenceValue = fd + 1.5*(c->GetHealth());
		}
		return true;
	}
	void GenRaidDeck(ActiveDeck &D, UINT RaidID)
	{
		_ASSERT(RaidID < RAID_MAX_ID);
		_ASSERT(RDB[RaidID].IsValid());
		D = ActiveDeck(&CDB[RDB[RaidID].GetCommander()]); // replace
		RDB[RaidID].GetAlways(CDB,D.Deck);
		RDB[RaidID].GetPools(CDB,D.Deck);
	}
	DWORD LoadPointers(Card **Ptr, DWORD MaxCount)
	{
		DWORD cnt = 0;
		for (DWORD i=0;(i<CARD_MAX_ID) && (cnt<MaxCount);i++)
			if (CDB[i].IsCard())
			{
				Ptr[cnt] = &CDB[i];
				cnt++;
			}
		return cnt;
	}
	void Initialize()
	{
		// must load skills earlier than anything
		AddSkill(ACTIVATION_CHAOS,"Chaos");
		AddSkill(ACTIVATION_CLEANSE,"Cleanse");
		AddSkill(ACTIVATION_ENFEEBLE,"Enfeeble");
		AddSkill(ACTIVATION_HEAL,"Heal");
		AddSkill(ACTIVATION_INFUSE,"Infuse");
		AddSkill(ACTIVATION_FREEZE,"Freeze");
		AddSkill(ACTIVATION_JAM,"Jam");
		AddSkill(ACTIVATION_MIMIC,"Mimic");
		AddSkill(ACTIVATION_PROTECT,"Protect");
		AddSkill(ACTIVATION_RALLY,"Rally");
		AddSkill(ACTIVATION_RECHARGE,"Recharge");
		AddSkill(ACTIVATION_REPAIR,"Repair");
		AddSkill(ACTIVATION_RUSH,"Rush");		
		AddSkill(ACTIVATION_SHOCK,"Shock");
		AddSkill(ACTIVATION_SIEGE,"Siege");
		AddSkill(ACTIVATION_SPLIT,"Split");
		AddSkill(ACTIVATION_STRIKE,"Strike");
		AddSkill(ACTIVATION_SUPPLY,"Supply");		
		AddSkill(ACTIVATION_WEAKEN,"Weaken");

		AddSkill(DEFENSIVE_ARMORED,"Armored");
		AddSkill(DEFENSIVE_COUNTER,"Counter");
		AddSkill(DEFENSIVE_EVADE,"Evade");
		AddSkill(DEFENSIVE_FLYING,"Flying");
		AddSkill(DEFENSIVE_INTERCEPT,"Intercept");
		AddSkill(DEFENSIVE_PAYBACK,"Payback");
		AddSkill(DEFENSIVE_REFRESH,"Refresh");
		AddSkill(DEFENSIVE_REGENERATE,"Regenerate");
		AddSkill(DEFENSIVE_TRIBUTE,"Tribute");		
		AddSkill(DEFENSIVE_WALL,"Wall");

		AddSkill(COMBAT_ANTIAIR,"AntiAir");
		AddSkill(COMBAT_BURST,"Burst");
		AddSkill(COMBAT_FEAR,"Fear");
		AddSkill(COMBAT_FLURRY,"Flurry");
		AddSkill(COMBAT_PIERCE,"Pierce");
		AddSkill(COMBAT_PIERCE,"Pierce_p"); // lets assume Pierce = Pierce_p
		AddSkill(COMBAT_SWIPE,"Swipe");
		AddSkill(COMBAT_VALOR,"Valor");

		AddSkill(DMGDEPENDANT_BERSERK,"Berserk");
		AddSkill(DMGDEPENDANT_CRUSH,"Crush");
		AddSkill(DMGDEPENDANT_DISEASE,"Disease");
		AddSkill(DMGDEPENDANT_IMMOBILIZE,"Immobilize");
		AddSkill(DMGDEPENDANT_LEECH,"Leech");
		AddSkill(DMGDEPENDANT_POISON,"Poison");
		AddSkill(DMGDEPENDANT_SIPHON,"Siphon");

		AddSkill(SPECIAL_BACKFIRE,"Backfire");

		AddSkill(SPECIAL_FUSION,"Fusion");
		AddSkill(SPECIAL_AUGMENT,"Augment");
		AddSkill(SPECIAL_MIST,"Mist");
		AddSkill(SPECIAL_BLIZZARD,"Blizzard");
	}
	void AddSkill(UCHAR Id, const char *Name)
	{
		char buffer[CARD_NAME_MAX_LENGTH];
		strcpy_s(buffer,CARD_NAME_MAX_LENGTH,Name);
		strlwr(buffer);		
		strcpy_s(Skills[Id].SkillName,CARD_NAME_MAX_LENGTH,Name);
		SIndex.insert(PAIRMSKILLS(buffer,Id));
	}
	const char *GetSkill(UCHAR Indx)
	{
		return Skills[Indx].SkillName;
	}
	UCHAR GetSkillID(const char *Name)
	{
		char buffer[CARD_NAME_MAX_LENGTH];
		strcpy_s(buffer,CARD_NAME_MAX_LENGTH,Name);
		strlwr(buffer);
		MSKILLS::iterator it = SIndex.find(buffer);
		if (it == SIndex.end())
			return 0;
		else
			return it->second;
	}
	UCHAR GetSkillIDSlow(const char *Name)
	{
		for (UCHAR i=0;i<CARD_ABILITIES_MAX;i++)
			if (!_strcmpi(Name,Skills[i].SkillName))
				return i;
		return 0; // not found
	}
	size_t GetSize()
	{
		return Index.size();
	}
	bool Insert(Card &c)
	{
		if (c.GetId())
		{
			if (c.GetId() > CARD_MAX_ID)
				throw 0;
			CDB[c.GetId()] = c;

			// so we need to add to index only those cards that have set - available for player
			pair<MSUINT::iterator, bool> mi = Index.insert(PAIRMSUINT((char*)c.GetName(),c.GetId()));
			if ((!mi.second) && (!CDB[mi.first->second].GetSet()))
				mi.first->second = c.GetId();
			return true;
		}
		return false;
	}
	bool CreateDeck(const char *CardsList, ActiveDeck &D)
	{
		char buffer[4096];
		strcpy_s(buffer,4096,CardsList);
		size_t len = strlen(buffer);
		size_t st = 0, cnt = 0;
		for (size_t i = 0; i < len+1; i++)
		{
			if ((buffer[i] == '"') || (buffer[i] == ',') || (!buffer[i]))
			{
				if ((buffer[i] == ',') && (!strnicmp(&buffer[st],CARD_NAME_SYNTAX_EXCLUSION,sizeof(CARD_NAME_SYNTAX_EXCLUSION))))
					continue;
				// tokenize
				buffer[i] = 0;
				if (st != i)
				{
					// token
					FormatCardName(&buffer[st]);
					const Card *c = GetCardSmart(&buffer[st]);
					if (c)
					{
						if (!cnt)
							D.Commander = c;
						else
							D.Deck.push_back(c);
						cnt++;
					}
				}
				st = i+1;
			}
		}
		return cnt > 0;
	}
	char *trim(char *str, char c=' ')
	{
		size_t len = strlen(str);
		int beg=0;
		while(str[len-1]==c) str[--len]=0;
		while(str[beg]==c) str[beg++]=0;
		if(beg) memmove(str, str+beg, len-beg+1);
		return str;
	}
	bool SaveCustomDecks(const char *FileName) { return SaveIndex(FileName,DIndex); }
	bool SaveMissionDecks(const char *FileName)
	{
		FILE *f;
		if (!fopen_s(&f,FileName,"w"))
		{
			for (UINT i=0;i<MISSION_MAX_ID;i++)
				if (MDB[i].GetCardCount())
				{				
					fprintf_s(f,"%s:",MDB[i].GetName());
					fprintf_s(f,"%s",CDB[MDB[i].GetCommander()].GetName());
					for (UINT k=0;k<MDB[i].GetCardCount();k++)
					{
						fprintf_s(f,",%s",CDB[MDB[i].GetCardID(k)].GetName());
					}
					fprintf_s(f,"\n");
				}
			return (fclose(f) == 0);
		}
		return false;
	}
	bool ClearDeckIndex()
	{
		DIndex.clear();
		return DIndex.empty();
	}
	int LoadDecks(const char *FileName, bool bRewrite = false)
	{
		FILE *f;
		int errline = -1, cline = 0; // no errors
		int Tag = TAG_CUSTOM;
		if (!fopen_s(&f,FileName,"r"))
		{
			// detect Tag
			UINT z=0,x=0;
			for (UINT i=0;FileName[i];i++)
				if (FileName[i] == '\\')
				{
					x = z;
					z = i;
				}
			if (x || z)
			{
				char buffer[50];
				memcpy(buffer,FileName+x+1,z-x-1);
				buffer[z-x-1] = 0;
				/*
#define TAG_ANY			-100500
#define TAG_CUSTOM		0
#define TAG_SOMERAID	-1000
#define TAG_SOMEMISSION	1000*/
				if (!stricmp(buffer,"CUSTOM"))
					Tag = TAG_CUSTOM;
				else
					if (!stricmp(buffer,"MISSION"))
						Tag = TAG_SOMEMISSION;
					else
						if (!stricmp(buffer,"RAID"))
							Tag = TAG_SOMERAID;
						else
							if (!stricmp(buffer,"BATCHEVAL"))
								Tag = TAG_BATCHEVAL;
			}
			//
			if (bRewrite)
			{
				DIndex.clear();
			}
#define MAX_BUFFER_SIZE	65535
			char buffer[MAX_BUFFER_SIZE];
			while (!feof(f))
			{
				bool bReadLine = false;
				fgets(buffer,MAX_BUFFER_SIZE,f);
				for (UINT i=0;(i<MAX_BUFFER_SIZE)&&(buffer[i]);i++)
					if (buffer[i] == '\n')
					{
						buffer[i] = 0;
						break;
					}
					else
						if (!isspace(buffer[i]))
							bReadLine = true;
				if ((buffer[0] != '/') && (buffer[1] != '/') && bReadLine) 
					if (!InsertDeck(Tag,buffer))
					{
						errline = cline;
						break;					
					}
				cline++;
			}
			fclose(f);
			return errline;
		}
		return -2; // fnf
	}
	int LoadOwnedCards(const char *FileName, MUUINT &OwnedCards)
	{
		FILE *f;
		int errline = -1, cline = 0; // no errors
		if (!fopen_s(&f,FileName,"r"))
		{
#define MAX_BUFFER_SIZE	65535
			char buffer[MAX_BUFFER_SIZE];
			while (!feof(f))
			{
				bool bReadLine = false;
				size_t brs = 0, bre = 0;
				fgets(buffer,MAX_BUFFER_SIZE,f);
				for (UINT i=0;(i<MAX_BUFFER_SIZE)&&(buffer[i]);i++)
					if (buffer[i] == '\n')
					{
						buffer[i] = 0;
						break;
					}
					else
						if (!isspace(buffer[i]))
						{
							bReadLine = true;
							if (buffer[i] == '(')
								brs = i;
							else
								if (buffer[i] == ')')
									bre = i;
						}
				if (!bReadLine)
					continue;
				UINT amount = 1;
				if ((brs > 0) && (bre > 0))
				{
					buffer[brs] = 0;
					buffer[bre] = 0;
					trim(buffer + brs + 1);
					amount = atoi(buffer + brs + 1);
					memset(buffer + brs,' ',bre - brs + 1);
				}
				trim(buffer);
				if ((buffer[0] != '/') && (buffer[1] != '/') && bReadLine) 
				{
					const Card *c = GetCardSmart(buffer);
					if ((!c) || (!c->IsCard()))
						errline = cline;
					else
						OwnedCards.insert(PAIRMUUINT::pair(c->GetId(),amount));
				}
				cline++;
			}
			fclose(f);
			return errline;
		}
		return -2; // fnf
	}
	int LoadCardList(const char *FileName, SCID &CardPool)
	{
		FILE *f;
		int errline = -1, cline = 0; // no errors
		if (!fopen_s(&f,FileName,"r"))
		{
#define MAX_BUFFER_SIZE	65535
			char buffer[MAX_BUFFER_SIZE];
			while (!feof(f))
			{
				bool bReadLine = false;
				fgets(buffer,MAX_BUFFER_SIZE,f);
				for (UINT i=0;(i<MAX_BUFFER_SIZE)&&(buffer[i]);i++)
					if (buffer[i] == '\n')
					{
						buffer[i] = 0;
						break;
					}
					else
						if (!isspace(buffer[i]))
							bReadLine = true;
				trim(buffer);
				if ((buffer[0] != '/') && (buffer[1] != '/') && bReadLine) 
				{
					const Card *c = GetCardSmart(buffer);
					if ((!c) || (!c->IsCard()))
						errline = cline;
					else
						CardPool.insert(c->GetId());
				}
				cline++;
			}
			fclose(f);
			return errline;
		}
		return -2; // fnf
	}
	bool InsertDeck(int Tag, const char *List, char *output_id_buffer = 0)
	{
		VSTRINGS cardlist;
		MDECKS *Into = &DIndex;
		MDECKS::iterator mi = Into->end();
		char buffer[4*CARD_NAME_MAX_LENGTH]; // somehow it's corrupted often, overflows :(
		size_t len = strlen(List);
		size_t p = 0,brs = 0,cnt = 0;
		size_t decknameend = 0;
		if (output_id_buffer)
			output_id_buffer[0] = 0;
		for (size_t i=0;i<len+1;i++)
			if ((List[i] == ',') || ((List[i] == ':') && (!p)) || (!List[i]))
			{
				if ((List[i] == ',') && (!strnicmp(List+p,CARD_NAME_SYNTAX_EXCLUSION,sizeof(CARD_NAME_SYNTAX_EXCLUSION))))
					continue;
				if (!p)
				{
					if (List[i] == ':')
					{
						decknameend = i;
						continue;
					}
					else
						if (decknameend > 0)
						{
							memcpy(buffer,List,decknameend);
							buffer[decknameend] = 0; // finalize string
							FormatCardName(buffer);
							if (output_id_buffer)
							{
								// we dont need deck name here
							}
							else
							{
								// detect IDs in square brackets [ ]
								int z=-1,x=-1;
								for (UINT v=0;buffer[v];v++)
									if (buffer[v] == '[')
										z = v;
									else
										if (buffer[v] == ']')
											x = v;
								int NewTag = 0;
								if ((z >= 0) && (x >= 0))
								{
									buffer[z]=0;
									buffer[x]=0;
									NewTag = atoi(trim(buffer + z + 1));
									strcat(buffer,trim(buffer+x+1)); // remove [id] from deck name
								}
								if (Tag == TAG_SOMERAID)
								{
									if (NewTag)
									{
										Tag = -NewTag; // negative tags for raids
									}
									else 
										if (!RIIndex.empty())
										{
											// try to detect by name
											for (MSUINT::iterator ri = RIIndex.begin(); ri != RIIndex.end(); ri++)
											{
												// full raid name
												const char *ptr = ri->first.c_str();
												if (strstr(buffer,ptr))
												{
													NewTag = ri->second; // found
													Tag = -NewTag;
													break;
												}
												// abbrevation
												char abv[10];
												UCHAR x=0;
												bool bStore = true;
												for (UINT z=0;ptr[z];z++)
													if (isspace(ptr[z]))
														bStore = true;
													else
														if (bStore)
														{
															abv[x] = ptr[z];
															x++;
															bStore = false;
														}
												if (!stricmp(ptr,"Blightbloom"))
												{
													abv[0] = 'B';
													abv[1] = 'B';
													x = 2;
												}
												if (x < 2) // it is pointless to search 1 char abbrevation
													continue;
												abv[x] = 0;
												const char *f = strstr(buffer,abv);
												while (f)
												{
													const char c = f[x];
													if (((f == buffer) || (isspace(*((unsigned char*)f-1)))) && ((!c) || (isspace(c))))
													{
														NewTag = ri->second; // found 
														break;
													}
													f = strstr(abv,ptr);
												}
												if (NewTag)
												{
													Tag = -NewTag;
													break;
												}
											}
										}
								}
								else
									if (Tag == TAG_SOMEMISSION)
									{
										if (NewTag)
										{
											Tag = NewTag; // positive tags for missions
										}
									}
								mi = Into->insert(PAIRMDECKS(DeckIndex(trim(buffer),Tag),cardlist)).first;
								mi->second.clear();
							}
							p = decknameend+1;
						}
				}
				memcpy(buffer,List+p*sizeof(char),i-p);
				buffer[i-p] = 0; // finalize string
				if (brs > 0)
				{
					for (size_t k = brs - p; k < i - p; k++)
					{
						if ((buffer[k] == '(') || (buffer[k] == ')') || isdigit(buffer[k]))
						{
							buffer[k] = ' ';
						}
					}
					brs = 0;
				}
				if ((output_id_buffer) || (mi != Into->end()))
				{
					// fix buffer
					for (UCHAR z=0;buffer[z];z++)
						if (buffer[z] == '’') // this char is so decieving ;(
							buffer[z] = '\'';
					FormatCardName(buffer);
					do
					{
						if (!output_id_buffer)
							mi->second.push_back(trim(buffer));
						else
						{
							const Card * c = GetCardSmart(trim(buffer));
							if (c)
							{
								char itoabuffer[10];
								itoa(c->GetId(),itoabuffer,10);
								if (output_id_buffer[0])
									strcat(output_id_buffer,",");
								strcat(output_id_buffer,itoabuffer);
							}
						}
						if (cnt)
							cnt--;
					}
					while (cnt > 0);
					cnt = 0;
				}
				p = i + 1;
			}
			else
				if (List[i] == '(')
					brs = i;
				else
					if ((List[i] == ')') && (brs > 0))
					{
						memcpy(buffer,List+(brs+1)*sizeof(char),i-brs-1);
						buffer[i-brs-1] = 0;
						cnt = atoi(buffer);
					}
		return (!output_id_buffer) || (output_id_buffer[0]);
	}
	void Print()
	{
		for (UINT i=0;i<CARD_MAX_ID;i++)
			if (CDB[i].IsCard())
			//if (it->second.GetName()[0]=='E')
			//	break;
			//else
			printf("%s\n",CDB[i].GetName());
#define MAX_ID_LEN	8
		char buffer[MAX_ID_LEN * DEFAULT_DECK_RESERVE_SIZE]; // buffer for ids
		buffer[0] = 0;
		for (MSUINT::iterator it=MIIndex.begin();it!=MIIndex.end();it++)
		{
			MDB[it->second].GetDeck(buffer,MAX_ID_LEN * DEFAULT_DECK_RESERVE_SIZE);
			printf("%s:%s\n",it->first.c_str(),buffer);
		}
		for (MDECKS::iterator it=DIndex.begin();it!=DIndex.end();it++)
		{
			printf("%s:",it->first.c_str());
			for (VSTRINGS::iterator et=it->second.begin();et!=it->second.end();et++)
			{
				if (et!=it->second.begin())
					printf(",");
				printf("%s",et->c_str());
			}
			printf("\n");
		}
	}
	const Card *CARD(const char *Name)
	{
		MSUINT::iterator it=Index.find(Name);
		if (it != Index.end())
			return &CDB[it->second];
		else
			return 0;
	}
	const Card *GetCard(string Name) { return GetCard(Name.c_str()); }
	const Card *GetCard(const char *Name)
	{
		MSUINT::iterator it=Index.find(Name);
		if (it != Index.end())
			return &CDB[it->second];
		else
		{
			printf("Can't find %s in database\n",Name);
			return 0;
		}
	}
	/*const int GetCardIndex(const char *Name)
	{
		MSUINT::iterator it=Index.find(Name);
		if (it != Index.end())
			return std::distance(Index.begin(), it);
		else
			return -1;
	}*/
	const Card &GetCard(const UINT Id)
	{
		if (Id > CARD_MAX_ID)
			throw 0;

		return CDB[Id];
	}
	const Card *GetCardSmart(const char *Name)
	{
		const char* i=strchr(Name,'[');
		if (!i)
			return GetCard(Name);
		const char* k=strchr(i,']');
		if (i && k)
		{
			char bu[10];
			memcpy(bu,i+1,k-i-1);
			bu[k-i-1] = 0;
			return &GetCard(atoi(bu));
		}
		return GetCard(Name);			
	}
	const bool GetCardListSorted(char *buffer, DWORD size)
	{
		string s;
		char lbuff[50];
		set<string> ss;
		for (UINT i=0;i<CARD_MAX_ID;i++)
			if (CDB[i].IsCard())
			{			
				s = CDB[i].GetName();
				if (!CDB[i].GetSet())
				{
					sprintf(lbuff,"[%d]",i);
					s += lbuff;
				}
				ss.insert(s);
			}
		s.clear();
		for (set<string>::iterator si = ss.begin(); si != ss.end(); si++)
		{
			if (!s.empty())
				s += ',';				
			s += '"' + *si + '"';
		}
		if (s.length() > size)
			return false;
		strcpy(buffer,s.c_str());
		return (!s.empty());
	}
	const char* GetCustomDecksList(char *buffer, size_t size, int byTag = TAG_ANY)
	{
		buffer[0] = 0;
		for (MDECKS::iterator mi = DIndex.begin(); mi != DIndex.end(); mi++)
		{
			if ((byTag != TAG_ANY) && (byTag != mi->first.Tag) && (byTag || (mi->first.Tag != TAG_BATCHEVAL))) // skip
				continue;
			if (buffer[0])
				strcat_s(buffer,size,","); // commatext :)
			strcat_s(buffer,size,"\"");
			strcat_s(buffer,size,mi->first.c_str());
			strcat_s(buffer,size,"\""); // yep, it's bad, but i dont care since it's called from delphi
		}
		return buffer;
	}
	const char* GetMissionDecksList(char *buffer, size_t size, bool bSortById = false)
	{
		buffer[0] = 0;
		if (bSortById)
			for (UINT id=0;id<MISSION_MAX_ID;id++)
			{
				if (MDB[id].IsValid())
				{
					if (buffer[0] != 0)
						strcat_s(buffer,size,","); // commatext :)
					strcat_s(buffer,size,"\"");
					strcat_s(buffer,size,MDB[id].GetName());
					strcat_s(buffer,size,"\""); // yep, it's bad, but i dont care since it's called from delphi
				}
			}
		else
			for (MSUINT::iterator mi = MIIndex.begin(); mi != MIIndex.end(); mi++)
			{
				if (mi != MIIndex.begin())
					strcat_s(buffer,size,","); // commatext :)
				strcat_s(buffer,size,"\"");
				strcat_s(buffer,size,mi->first.c_str());
				strcat_s(buffer,size,"\""); // yep, it's bad, but i dont care since it's called from delphi
			}
		return buffer;
	}
	const char* GetCustomDeck(const char* DeckName, const int Tag, char *buffer, size_t size)
	{
		buffer[0] = 0;
		MDECKS::iterator mi = DIndex.find(DeckIndex(DeckName,Tag));
		if ((!Tag) && (mi == DIndex.end()))
			 mi = DIndex.find(DeckIndex(DeckName,TAG_BATCHEVAL));			
		if (mi == DIndex.end())	
			return buffer; // not found
		if (mi->second.empty())
			return buffer; // empty deck :(
		for (VSTRINGS::iterator vi = mi->second.begin();vi != mi->second.end();vi++)
		{
			if (vi != mi->second.begin())
				strcat_s(buffer,size,","); // commatext :)
			strcat_s(buffer,size,"\"");
			strcat_s(buffer,size,(*vi).c_str());
			strcat_s(buffer,size,"\""); // yep, it's bad, but i dont care since it's called from delphi
		}
		return buffer;
	}
	const char* GetMissionDeck(const char* DeckName, char *buffer, size_t size)
	{
		buffer[0] = 0;
		MSUINT::iterator mi = MIIndex.find(DeckName);
		if (mi == MIIndex.end())	
			return buffer; // not found
		itoa(MDB[mi->second].GetCommander(),buffer,10);
		MDB[mi->second].GetDeck(buffer,size);
		return buffer;
	}
	const UINT GetMissionDeckIndex(const char* DeckName)
	{
		MSUINT::iterator mi = MIIndex.find(DeckName);
		if (mi == MIIndex.end())	
			return 0; // not found
		return mi->second;
	}
	const char* GetRaidDecksList(char *buffer, size_t size)
	{
		buffer[0] = 0;
		for (UCHAR i=0;i<RAID_MAX_ID;i++)
			if (RDB[i].IsValid())
			{
				if (buffer[0] != 0)	
					strcat_s(buffer,size,","); // commatext :)
				strcat_s(buffer,size,"\"");
				strcat_s(buffer,size,RDB[i].GetName());
				strcat_s(buffer,size,"\""); // yep, it's bad, but i dont care since it's called from delphi
			}
		return buffer;
	}
	const UINT GetRaidCommanderID(UINT RaidIndex)
	{
		return RDB[RaidIndex].GetCommander();
	}
	void GenRandomDeckFromCard(VCARDS &Deck,size_t i)
	{
		MSUINT::iterator mi = Index.begin();
		for (; mi != Index.end(); mi++)
		{
			if ((!i) && (CDB[mi->second].GetRarity() < RARITY_UNIQUE))
				break;
			else
				i--;
		}
		if (mi != Index.end())
			for (UCHAR k=0;k<DEFAULT_DECK_SIZE;k++)
				Deck.push_back(&CDB[mi->second]);
	}
	const UINT GetCustomCount()
	{
		UINT c = 0;
		for (MDECKS::iterator mi = DIndex.begin(); mi != DIndex.end(); mi++)
			//if (mi->first.Tag == TAG_CUSTOM) GET ANY COUNT
				c++;
		return c;
	}
	bool GetCustomDeck(UINT Index, ActiveDeck &R)
	{
		for (MDECKS::iterator mi = DIndex.begin(); mi != DIndex.end(); mi++)
			//if (mi->first.Tag == TAG_CUSTOM) GET ANY DECK
			{
				if (!Index)
				{
					if (mi->second.empty())
						return false;
					VSTRINGS::iterator vi = mi->second.begin();
					R.Commander = GetCardSmart(vi->c_str());
					R.Deck.clear();
					for (vi++;vi != mi->second.end();vi++)
						R.Add(GetCardSmart(vi->c_str()));
					return true;
				}
				Index--;
			}
		return false;
	}
	// named decks
	ActiveDeck GetNamedDeck(const char* DeckName, const int Tag)
	{
		MDECKS::iterator mi = DIndex.find(DeckIndex(DeckName,Tag));
		if (mi == DIndex.end())	
			printf("Can't find deck %s in database\n",DeckName);
		_ASSERT(mi != DIndex.end());
		if (mi->second.empty())
			printf("Deck %s has 0 cards\n",DeckName);
		_ASSERT(!mi->second.empty());
		VSTRINGS::iterator vi = mi->second.begin();
		ActiveDeck r(GetCard(*vi));
		for (vi++;vi != mi->second.end();vi++)
			r.Add(GetCard(*vi));
		return r;
	}
};