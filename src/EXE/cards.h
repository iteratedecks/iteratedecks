#include <map>
#include <vector>
#include <string>
#include "pugixml\src\pugixml.hpp"
#include "pugixml\src\pugixml.cpp"

typedef unsigned long       DWORD;

typedef map<string, UINT> MSUINT;
typedef pair<string, UINT> PAIRMSUINT;
typedef vector<string> VSTRINGS;
typedef map<string, VSTRINGS> MDECKS;
typedef pair<string, VSTRINGS> PAIRMDECKS;
typedef map<string, UCHAR> MSKILLS;
typedef pair<string, UCHAR> PAIRMSKILLS;
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
			indx = rand() % fromIDpool.size();
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
class CardDB
{	
	Card CDB[CARD_MAX_ID]; // this can cause stack overflow, but should work fastest
	MissionInfo MDB[MISSION_MAX_ID];
	RaidInfo RDB[RAID_MAX_ID];
	MSUINT MIIndex;
	MSUINT RIIndex;
	MSUINT Index;
	MDECKS DIndex;
	char SKILLS[CARD_ABILITIES_MAX][CARD_NAME_MAX_LENGTH];
	MSKILLS SIndex;
public://protected:
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
			if (!_strcmpi(it->name(),"unit"))
			{
				const char *Name = it->child("name").child_value();
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
						UCHAR Effect = child.attribute("x").as_uint();
						if (!Effect)
							Effect = ABILITY_ENABLED; // this value can't be 0, since it will disable the ability
						UCHAR TC = child.attribute("all").as_uint();
						if (!TC)
							TC = TARGETSCOUNT_ONE;
						else
							TC = TARGETSCOUNT_ALL;
						UCHAR TF = RemapFaction(child.attribute("y").as_uint());
						c.AddAbility(Id,Effect,TC,TF);
						//for (pugi::xml_attribute attr = child.first_attribute(); attr; attr = attr.next_attribute())
							//printf("    %s = %s\n",attr.name(),attr.value());
					}
				}
				//MSUINT::iterator mi = Index.find(c.GetName());
				//if (mi != Index.end())
				//	printf("Conflicted: %s : %d and %d\n",c.GetName(),mi->second,c.GetId());
				// so card should be treated as new if it's picture changed, so API would know it should download new pic
				bool isnew = ((!CDB[Id].IsCard()) || (stricmp(CDB[Id].GetPicture(),c.GetPicture())));
				bool ins = Insert(c,(Set > 0));
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
		// must load skills before than anything
		for (UCHAR i=0;i<CARD_ABILITIES_MAX;i++)
			SKILLS[i][0] = 0;
		AddSkill(ACTIVATION_ENFEEBLE,"Enfeeble");
		AddSkill(ACTIVATION_HEAL,"Heal");
		AddSkill(ACTIVATION_INFUSE,"Infuse");
		AddSkill(ACTIVATION_JAM,"Jam");
		AddSkill(ACTIVATION_MIMIC,"Mimic");
		AddSkill(ACTIVATION_RALLY,"Rally");
		AddSkill(ACTIVATION_RECHARGE,"Recharge");
		AddSkill(ACTIVATION_REPAIR,"Repair");
		AddSkill(ACTIVATION_SHOCK,"Shock");
		AddSkill(ACTIVATION_SIEGE,"Siege");
		AddSkill(ACTIVATION_SPLIT,"Split");
		AddSkill(ACTIVATION_STRIKE,"Strike");
		AddSkill(ACTIVATION_WEAKEN,"Weaken");

		AddSkill(DEFENSIVE_ARMORED,"Armored");
		AddSkill(DEFENSIVE_COUNTER,"Counter");
		AddSkill(DEFENSIVE_EVADE,"Evade");
		AddSkill(DEFENSIVE_FLYING,"Flying");
		AddSkill(DEFENSIVE_PAYBACK,"Payback");
		AddSkill(DEFENSIVE_REGENERATE,"Regenerate");
		AddSkill(DEFENSIVE_WALL,"Wall");

		AddSkill(COMBAT_ANTIAIR,"AntiAir");
		AddSkill(COMBAT_FEAR,"Fear");
		AddSkill(COMBAT_FLURRY,"Flurry");
		AddSkill(COMBAT_PIERCE,"Pierce");
		AddSkill(COMBAT_SWIPE,"Swipe");
		AddSkill(COMBAT_VALOR,"Valor");

		AddSkill(DMGDEPENDANT_CRUSH,"Crush");
		AddSkill(DMGDEPENDANT_IMMOBILIZE,"Immobilize");
		AddSkill(DMGDEPENDANT_LEECH,"Leech");
		AddSkill(DMGDEPENDANT_POISON,"Poison");
		AddSkill(DMGDEPENDANT_SIPHON,"Siphon");

		AddSkill(SPECIAL_BACKFIRE,"Backfire");

		AddSkill(SPECIAL_FUSION,"Fusion");
		AddSkill(SPECIAL_BERSERK,"Berserk");
		AddSkill(SPECIAL_PROTECTION,"Protection");
		AddSkill(SPECIAL_AUGMENT,"Augment");
		AddSkill(SPECIAL_MIST,"Mist");
	}
	void AddSkill(UCHAR Id, const char *Name)
	{
		char buffer[CARD_NAME_MAX_LENGTH];
		strcpy_s(buffer,CARD_NAME_MAX_LENGTH,Name);
		strlwr(buffer);
		strcpy_s(SKILLS[Id],CARD_NAME_MAX_LENGTH,Name);
		SIndex.insert(PAIRMSKILLS(buffer,Id));
	}
	const char *GetSkill(UCHAR Indx)
	{
		return SKILLS[Indx];
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
			if (!_strcmpi(Name,SKILLS[i]))
				return i;
		return 0; // not found
	}
	size_t GetSize()
	{
		return Index.size();
	}
	bool Insert(Card &c, bool bOverwrite=false)
	{
		if (c.GetId())
		{
			if (c.GetId() > CARD_MAX_ID)
				throw 0;
			CDB[c.GetId()] = c;

			// so we need to add to index only those cards that have set - available for player
			pair<MSUINT::iterator, bool> mi = Index.insert(PAIRMSUINT((char*)c.GetName(),c.GetId()));
			if ((!mi.second) && bOverwrite)
				mi.first->second = c.GetId();
			return true;
		}
		return false;
	}
	void CreateDeck(const char *CardsList, ActiveDeck &D)
	{
		char buffer[4096];
		strcpy_s(buffer,4096,CardsList);
		size_t len = strlen(buffer);
		size_t st = 0, cnt = 0;
		for (size_t i = 0; i < len+1; i++)
		{
			if ((buffer[i] == '"') || (buffer[i] == ',') || (!buffer[i]))
			{
				// tokenize
				buffer[i] = 0;
				if (st != i)
				{
					// token
					if (!cnt)
						D.Commander = &GetCardSmart(&buffer[st]);
					else
						D.Deck.push_back(&GetCardSmart(&buffer[st]));
					cnt++;
				}
				st = i+1;
			}
		}
	}
	char *trim(char *str, char c=' ')
	{
		int len = strlen(str);
		int beg=0;
		while(str[len-1]==c) str[--len]=0;
		while(str[beg]==c) str[beg++]=0;
		if(beg) memmove(str, str+beg, len-beg+1);
		return str;
	}
	bool SaveCustomDecks(const char *FileName) { return SaveIndex(FileName,DIndex); }
	int LoadDecks(const char *FileName, bool bRewrite = false)
	{
		FILE *f;
		int errline = -1, cline = 0; // no errors
		if (!fopen_s(&f,FileName,"r"))
		{
			if (bRewrite)
			{
				DIndex.clear();
			}
#define MAX_BUFFER_SIZE	65535
			char buffer[MAX_BUFFER_SIZE];
			while (!feof(f))
			{
				fgets(buffer,MAX_BUFFER_SIZE,f);
				for (UINT i=0;(i<MAX_BUFFER_SIZE)&&(buffer[i]);i++)
					if (buffer[i] == '\n')
						buffer[i] = 0;
				if ((buffer[0] != '/') && (buffer[1] != '/')) 
					if (!InsertDeck(buffer))
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
	bool InsertDeck(const char *List)
	{
		MDECKS::iterator mi;
		VSTRINGS cardlist;
		MDECKS *Into = &DIndex;
		char buffer[4*CARD_NAME_MAX_LENGTH]; // somehow it's corrupted often, overflows :(
		size_t len = strlen(List);
		size_t p = 0,brs = 0,cnt = 0;
		try
		{
			for (size_t i=0;i<len+1;i++)
				if ((List[i] == ',') || (List[i] == ':') || (!List[i]))
				{
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
					if (!p)
					{
						mi = Into->insert(PAIRMDECKS(trim(buffer),cardlist)).first;
						mi->second.clear();
					}
					else
						if (mi != Into->end())
						{
							do
							{
								mi->second.push_back(trim(buffer));
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
			return true;
		}
		catch(char * /* str*/) 
		{
			//cout << "Exception raised: " << str << '\n';
			return false;
		}
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
	const Card *CARD(const char *Name) { return &GetCard(Name); }
	const Card &GetCard(string Name) { return GetCard(Name.c_str()); }
	const Card &GetCard(const char *Name)
	{
		MSUINT::iterator it=Index.find(Name);
		if (it != Index.end())
			return CDB[it->second];
		else
		{
			printf("Can't find %s in database\n",Name);
			_ASSERT(it != Index.end());
			it = Index.lower_bound(Name); // get lower bound instead
			return CDB[it->second];
		}
	}
	const Card &GetCard(const UINT Id)
	{
		if (Id > CARD_MAX_ID)
			throw 0;

		return CDB[Id];
	}
	const Card &GetCardSmart(const char *Name)
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
			return GetCard(atoi(bu));
		}
		return GetCard(Name);			
	}
	const char* GetCustomDecksList(char *buffer, size_t size)
	{
		buffer[0] = 0;
		for (MDECKS::iterator mi = DIndex.begin(); mi != DIndex.end(); mi++)
		{
			if (mi != DIndex.begin())
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
	const char* GetCustomDeck(const char* DeckName, char *buffer, size_t size)
	{
		buffer[0] = 0;
		MDECKS::iterator mi = DIndex.find(DeckName);
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
			for (UCHAR k=0;k<10;k++)
				Deck.push_back(&CDB[mi->second]);
	}
	/*Card &GenRandomDeck(VCARDS &Deck) // returns commander
	{
		MSUINT::iterator mi;
		bool bLegendary = false;
		UCHAR p;
		Deck.clear();
		for (UCHAR i=0;i<10;i++)
		{
			do
			{
				mi = Index.begin();	
				p = rand() % Index.size();
				for (UCHAR k=0;k<p;k++)
					mi ++;
			}
			while ((CDB[mi->second].GetType() == TYPE_COMMANDER) 
				|| (bLegendary && (CDB[mi->second].GetRarity() == RARITY_LEGENDARY))
				|| ((CDB[mi->second].GetRarity() == RARITY_UNIQUE) && IsCardInDeck(CDB[mi->second].GetName(),Deck)));
			Deck.push_back(&CDB[mi->second]);
			if (CDB[mi->second].GetRarity() == RARITY_LEGENDARY)
				bLegendary = true;
		}
		//
		do
		{
			mi = Index.begin();
			p = rand() % Index.size();
			for (UCHAR k=0;k<p;k++)
				mi ++;
		}
		while (CDB[mi->second].GetType() != TYPE_COMMANDER);
		return (CDB[mi->second]);
	}*/
	// named decks
	ActiveDeck GetNamedDeck(const char* DeckName)
	{
		MDECKS::iterator mi = DIndex.find(DeckName);
		if (mi == DIndex.end())	
			printf("Can't find deck %s in database\n",DeckName);
		_ASSERT(mi != DIndex.end());
		if (mi->second.empty())
			printf("Deck %s has 0 cards\n",DeckName);
		_ASSERT(!mi->second.empty());
		VSTRINGS::iterator vi = mi->second.begin();
		ActiveDeck r(&GetCard(*vi));
		for (vi++;vi != mi->second.end();vi++)
			r.Add(&GetCard(*vi));
		return r;
	}
	// raid decks
	/*void GenXenoWalkerDeck(VCARDS &XenoWalkerDeck)
	{
		// Gen Xeno Walker deck
		XenoWalkerDeck.push_back(CARD("Energy Cannon"));
		XenoWalkerDeck.push_back(CARD("Lightning Cannon"));
		XenoWalkerDeck.push_back(CARD("Ixnedrone"));
		XenoWalkerDeck.push_back(CARD("Ixnedrone"));
		XenoWalkerDeck.push_back(CARD("Sustainer Xolan"));
		XenoWalkerDeck.push_back(CARD("Sustainer Xolan"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Invasion Scouts"));
		tmppool.push_back(CARD("Abolisher"));
		tmppool.push_back(CARD("Gravity Tank"));
		tmppool.push_back(CARD("Mobile Extractor"));
		tmppool.push_back(CARD("Predator"));
		tmppool.push_back(CARD("Predator"));
		tmppool.push_back(CARD("Speculus"));
		tmppool.push_back(CARD("Speculus"));

		PickACard(tmppool,XenoWalkerDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Speculus"));
		tmppool.push_back(CARD("Dominated Hatchlings"));
		tmppool.push_back(CARD("Enclave Champion"));
		tmppool.push_back(CARD("Enclave Champion"));
		tmppool.push_back(CARD("Ghost"));
		tmppool.push_back(CARD("Pathrazer"));
		tmppool.push_back(CARD("Stealthy Niaq"));
		tmppool.push_back(CARD("Stealthy Niaq"));

		PickACard(tmppool,XenoWalkerDeck);
		PickACard(tmppool,XenoWalkerDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Blinding Apprentice"));
		tmppool.push_back(CARD("Daemon"));
		tmppool.push_back(CARD("Enclave Warlord"));
		tmppool.push_back(CARD("Mind Controller"));
		tmppool.push_back(CARD("Monstrosity"));
		tmppool.push_back(CARD("Rifter"));
		tmppool.push_back(CARD("X-5 Penetrator"));

		PickACard(tmppool,XenoWalkerDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Daemon"));
		tmppool.push_back(CARD("Dragoon Hunters"));
		tmppool.push_back(CARD("Dragoon Hunters"));
		tmppool.push_back(CARD("Kraken"));
		tmppool.push_back(CARD("Tremor Wyrm"));
		tmppool.push_back(CARD("World Quaker"));

		PickACard(tmppool,XenoWalkerDeck);
		PickACard(tmppool,XenoWalkerDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Enclave Pylon"));
		tmppool.push_back(CARD("Mirror Wall"));
		tmppool.push_back(CARD("SkyCom"));
		tmppool.push_back(CARD("Panopticon"));
		tmppool.push_back(CARD("Chaos Wave"));
		tmppool.push_back(CARD("Mend Wounds"));
		tmppool.push_back(CARD("Death from Above"));

		PickACard(tmppool,XenoWalkerDeck);
		PickACard(tmppool,XenoWalkerDeck);
		PickACard(tmppool,XenoWalkerDeck);

		tmppool.clear(); // cleanup
	}
	void GenSiegeOnKorDeck(VCARDS &SiegeOnKorDeck)
	{
		// Gen Siege On Kor deck
		SiegeOnKorDeck.push_back(CARD("Engineer"));
		SiegeOnKorDeck.push_back(CARD("Mammoth Tank"));
		SiegeOnKorDeck.push_back(CARD("Landmine"));
		SiegeOnKorDeck.push_back(CARD("Landmine"));
		SiegeOnKorDeck.push_back(CARD("Gatling Tower"));
		SiegeOnKorDeck.push_back(CARD("Gatling Tower"));
		SiegeOnKorDeck.push_back(CARD("Cannon Wall"));
		SiegeOnKorDeck.push_back(CARD("Cannon Wall"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Cannon Walker"));
		tmppool.push_back(CARD("Cannon Walker"));
		tmppool.push_back(CARD("Hunter"));
		tmppool.push_back(CARD("Hydroblade"));
		tmppool.push_back(CARD("Lodestar"));

		PickACard(tmppool,SiegeOnKorDeck);
		PickACard(tmppool,SiegeOnKorDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Mech Walker"));
		tmppool.push_back(CARD("Fury Walker"));
		tmppool.push_back(CARD("Hydra"));
		tmppool.push_back(CARD("Havoc"));
		tmppool.push_back(CARD("Pelican"));
		tmppool.push_back(CARD("Pelican"));

		PickACard(tmppool,SiegeOnKorDeck);
		PickACard(tmppool,SiegeOnKorDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Nightstalker"));
		tmppool.push_back(CARD("Chronos"));
		tmppool.push_back(CARD("Chronos"));
		tmppool.push_back(CARD("Obsidian"));
		tmppool.push_back(CARD("Dozer Tank"));
		tmppool.push_back(CARD("Anvil"));
		tmppool.push_back(CARD("Colossus"));
		tmppool.push_back(CARD("Colossus"));
		tmppool.push_back(CARD("Bulldozer"));
		tmppool.push_back(CARD("Bulldozer"));
		tmppool.push_back(CARD("Barracuda"));

		PickACard(tmppool,SiegeOnKorDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Bolide Walker"));
		tmppool.push_back(CARD("Pyro Rig"));
		tmppool.push_back(CARD("Gun Raven"));
		tmppool.push_back(CARD("Vorpal Tank"));
		tmppool.push_back(CARD("Razor"));
		tmppool.push_back(CARD("Razor"));
		tmppool.push_back(CARD("Mammoth Tank"));

		PickACard(tmppool,SiegeOnKorDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Colossus"));
		tmppool.push_back(CARD("Gun Raven"));
		tmppool.push_back(CARD("Earthquake Generator"));
		tmppool.push_back(CARD("Missile Silo"));
		tmppool.push_back(CARD("Tesla Cannon"));

		PickACard(tmppool,SiegeOnKorDeck);

		tmppool.clear(); // cleanup
	}
	void GenImperialPurgerDeck(VCARDS &ImperialPurgerDeck)
	{
		// Gen Siege On Kor deck
		ImperialPurgerDeck.push_back(CARD("GDR Battle Suit"));
		ImperialPurgerDeck.push_back(CARD("GDR Battle Suit"));
		ImperialPurgerDeck.push_back(CARD("GDR Infantry"));
		ImperialPurgerDeck.push_back(CARD("GDR Infantry"));
		ImperialPurgerDeck.push_back(CARD("GDR Gatling Gun"));
		ImperialPurgerDeck.push_back(CARD("GDR Core"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Sawblade"));
		tmppool.push_back(CARD("Tiamat"));
		tmppool.push_back(CARD("Tiamat"));
		tmppool.push_back(CARD("Arc Trooper"));
		tmppool.push_back(CARD("Arc Trooper"));
		tmppool.push_back(CARD("Hornet Drones"));
		tmppool.push_back(CARD("Ion Strykers"));
		tmppool.push_back(CARD("Irradiated Infantry"));
		tmppool.push_back(CARD("Mend Wounds"));
		tmppool.push_back(CARD("Regeneration Bay"));

		PickACard(tmppool,ImperialPurgerDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Arc Trooper"));
		tmppool.push_back(CARD("Quad Walker"));
		tmppool.push_back(CARD("Sabre"));
		tmppool.push_back(CARD("Exodrone"));
		tmppool.push_back(CARD("Exodrone"));
		tmppool.push_back(CARD("Tactical Infiltrator"));
		tmppool.push_back(CARD("Trident"));
		tmppool.push_back(CARD("Trident"));
		tmppool.push_back(CARD("Sharpshooter"));
		tmppool.push_back(CARD("Sharpshooter"));
		tmppool.push_back(CARD("Swift Troops"));
		tmppool.push_back(CARD("Jet Trooper"));

		PickACard(tmppool,ImperialPurgerDeck);
		PickACard(tmppool,ImperialPurgerDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Exodrone"));
		tmppool.push_back(CARD("Terminator"));
		tmppool.push_back(CARD("Stormrunner"));
		tmppool.push_back(CARD("Aegis"));
		tmppool.push_back(CARD("Aegis"));
		tmppool.push_back(CARD("Fortifier"));
		tmppool.push_back(CARD("Fortifier"));
		tmppool.push_back(CARD("Fortifier"));
		tmppool.push_back(CARD("Poseidon"));
		tmppool.push_back(CARD("Poseidon"));

		PickACard(tmppool,ImperialPurgerDeck);
		PickACard(tmppool,ImperialPurgerDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Stormrunner"));
		tmppool.push_back(CARD("Ion Strykers"));
		tmppool.push_back(CARD("Titan"));
		tmppool.push_back(CARD("Titan"));
		tmppool.push_back(CARD("Reaper"));
		tmppool.push_back(CARD("Heavy Gunner"));
		tmppool.push_back(CARD("Manta"));
		tmppool.push_back(CARD("Immortal"));

		PickACard(tmppool,ImperialPurgerDeck);
		PickACard(tmppool,ImperialPurgerDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Petrol Rig"));
		tmppool.push_back(CARD("Absorption Shield"));
		tmppool.push_back(CARD("Absorption Shield"));
		tmppool.push_back(CARD("ComSat Terminal"));
		tmppool.push_back(CARD("Airstrike"));
		tmppool.push_back(CARD("Airstrike"));
		tmppool.push_back(CARD("Electromagnetic Pulse"));
		tmppool.push_back(CARD("Maximum Damage"));
		tmppool.push_back(CARD("Full Power"));

		PickACard(tmppool,ImperialPurgerDeck);
		PickACard(tmppool,ImperialPurgerDeck);

		tmppool.clear(); // cleanup
	}
	void GenArctisVanguardDeck(VCARDS &ArctisVanguardDeck)
	{
		// Gen Arctis Vanguard deck
		ArctisVanguardDeck.push_back(CARD("Safeguard"));
		ArctisVanguardDeck.push_back(CARD("Cyclone"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Raider Elite"));
		tmppool.push_back(CARD("Raider Elite"));
		tmppool.push_back(CARD("Biped"));
		tmppool.push_back(CARD("Biped"));
		tmppool.push_back(CARD("Gunjack"));
		tmppool.push_back(CARD("Gunjack"));
		tmppool.push_back(CARD("Raider Hawkeye"));
		tmppool.push_back(CARD("Raider Hawkeye"));
		tmppool.push_back(CARD("Dread Panzer"));
		tmppool.push_back(CARD("Raider Wall"));
		tmppool.push_back(CARD("Foundry"));

		PickACard(tmppool,ArctisVanguardDeck);
		PickACard(tmppool,ArctisVanguardDeck);
		PickACard(tmppool,ArctisVanguardDeck);
		PickACard(tmppool,ArctisVanguardDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Headhunter"));
		tmppool.push_back(CARD("Bombardment Tanks"));
		tmppool.push_back(CARD("Combat Specialist"));
		tmppool.push_back(CARD("Hyperion Prototype"));
		tmppool.push_back(CARD("Shock Grunt"));
		tmppool.push_back(CARD("Artemis Tank"));

		PickACard(tmppool,ArctisVanguardDeck);
		PickACard(tmppool,ArctisVanguardDeck);

		tmppool.clear(); // cleanup
	}
	void GenEnclaveFlagshipDeck(VCARDS &EnclaveFlagshipDeck)
	{
		// Gen Enclave Flagship deck
		EnclaveFlagshipDeck.push_back(&CDB[218]); // azure reaper
		EnclaveFlagshipDeck.push_back(&CDB[218]); // azure reaper
		EnclaveFlagshipDeck.push_back(CARD("Enclave Trooper"));
		EnclaveFlagshipDeck.push_back(CARD("Enclave Trooper"));
		EnclaveFlagshipDeck.push_back(CARD("Battle Station"));
		EnclaveFlagshipDeck.push_back(CARD("Xeno Poison Bomb"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Enclave Trooper"));
		tmppool.push_back(&CDB[218]); // azure reaper
		tmppool.push_back(CARD("Dominated Hatchlings"));
		tmppool.push_back(CARD("Dominated Hatchlings"));
		tmppool.push_back(CARD("Dreadship"));
		tmppool.push_back(CARD("Ghost"));
		tmppool.push_back(CARD("Vaporwing"));
		tmppool.push_back(CARD("Vaporwing"));

		PickACard(tmppool,EnclaveFlagshipDeck);
		PickACard(tmppool,EnclaveFlagshipDeck);
		PickACard(tmppool,EnclaveFlagshipDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Xeno Interceptor"));
		tmppool.push_back(CARD("Xeno Interceptor"));
		tmppool.push_back(CARD("Phoenix"));
		tmppool.push_back(CARD("Phoenix"));
		tmppool.push_back(CARD("Daemon"));

		PickACard(tmppool,EnclaveFlagshipDeck);
		PickACard(tmppool,EnclaveFlagshipDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Rifter"));
		tmppool.push_back(CARD("Rifter"));
		tmppool.push_back(CARD("Xeno Mothership"));
		tmppool.push_back(CARD("Xeno Mothership"));
		tmppool.push_back(CARD("Enclave Remnant"));

		PickACard(tmppool,EnclaveFlagshipDeck);
		PickACard(tmppool,EnclaveFlagshipDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Daemon"));
		tmppool.push_back(CARD("Battle Station"));
		tmppool.push_back(CARD("Death from Above"));

		PickACard(tmppool,EnclaveFlagshipDeck);
		PickACard(tmppool,EnclaveFlagshipDeck);

		tmppool.clear(); // cleanup
	}
	void GenOluthDeck(VCARDS &OluthDeck)
	{
		// Gen Oluth deck
		OluthDeck.push_back(CARD("Venomous Restrictor"));
		OluthDeck.push_back(CARD("Venomous Restrictor"));
		OluthDeck.push_back(CARD("Grapplers"));
		OluthDeck.push_back(CARD("Grapplers"));
		OluthDeck.push_back(CARD("Mystic Tentacle"));
		OluthDeck.push_back(CARD("Tentacle"));
		OluthDeck.push_back(CARD("Tentacle"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Annelid Mass"));
		tmppool.push_back(CARD("Moloch"));
		tmppool.push_back(CARD("Moloch"));
		tmppool.push_back(CARD("Rabid Corruptor"));
		tmppool.push_back(CARD("Shapeshifter"));
		tmppool.push_back(CARD("Shapeshifter"));
		tmppool.push_back(CARD("Shapeshifter"));
		tmppool.push_back(CARD("Carcass Scrounge"));
		tmppool.push_back(CARD("Carcass Scrounge"));
		tmppool.push_back(CARD("Blood Spout"));

		PickACard(tmppool,OluthDeck);
		PickACard(tmppool,OluthDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Brood Walker"));
		tmppool.push_back(CARD("Blood Spout"));
		tmppool.push_back(CARD("Blood Spout"));
		tmppool.push_back(CARD("Desolate Sage"));
		tmppool.push_back(CARD("Venomous Raptor"));
		tmppool.push_back(CARD("Venomous Raptor"));

		PickACard(tmppool,OluthDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Ravager"));
		tmppool.push_back(CARD("Sarcous Grub"));
		tmppool.push_back(CARD("Feral Scorpion"));
		tmppool.push_back(CARD("Feral Scorpion"));
		tmppool.push_back(CARD("Acid Spewer"));
		tmppool.push_back(CARD("Mawcor"));
		tmppool.push_back(CARD("Mawcor"));
		tmppool.push_back(CARD("Desolate Sage"));

		PickACard(tmppool,OluthDeck);
		PickACard(tmppool,OluthDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Draconian Queen"));
		tmppool.push_back(CARD("Draconian Queen"));
		tmppool.push_back(CARD("Pummeller"));
		tmppool.push_back(CARD("Destructive Behemoth"));
		tmppool.push_back(CARD("Nightmare"));
		tmppool.push_back(CARD("Gore Crawler"));

		PickACard(tmppool,OluthDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Pummeller"));
		tmppool.push_back(CARD("Blood Pool"));
		tmppool.push_back(CARD("Blood Pool"));
		tmppool.push_back(CARD("Blood Pool"));
		tmppool.push_back(CARD("Sulfuris Acid Pit"));
		tmppool.push_back(CARD("Blood Wall"));
		tmppool.push_back(CARD("Festering Wasteland"));
		tmppool.push_back(CARD("Blight Tower"));
		tmppool.push_back(CARD("Blight Tower"));
		tmppool.push_back(CARD("Irradiation"));
		tmppool.push_back(CARD("Irradiation"));
		tmppool.push_back(CARD("Pandemic"));

		PickACard(tmppool,OluthDeck);
		PickACard(tmppool,OluthDeck);

		tmppool.clear(); // cleanup
	}
	void GenTartarusSwarmDeck(VCARDS &TartarusSwarmDeck)
	{
		// Gen Tartarus Swarm deck
		TartarusSwarmDeck.push_back(CARD("Spawn"));
		TartarusSwarmDeck.push_back(CARD("Spawn"));
		TartarusSwarmDeck.push_back(&CDB[236]); // carrion retriever
		TartarusSwarmDeck.push_back(CARD("Destructive Behemoth"));
		TartarusSwarmDeck.push_back(CARD("Mutated Lieutenant"));
		TartarusSwarmDeck.push_back(&CDB[3013]); // acid splash
		TartarusSwarmDeck.push_back(CARD("Tentacle"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Annelid Mass"));
		tmppool.push_back(CARD("Spawn"));
		tmppool.push_back(CARD("Vampire"));
		tmppool.push_back(CARD("Blood Grunt"));
		tmppool.push_back(CARD("Hatchet"));
		tmppool.push_back(CARD("Hatchet"));
		tmppool.push_back(CARD("Hatchet"));
		tmppool.push_back(CARD("Shapeshifter"));
		tmppool.push_back(CARD("Shapeshifter"));
		tmppool.push_back(CARD("Groteske"));

		PickACard(tmppool,TartarusSwarmDeck);
		PickACard(tmppool,TartarusSwarmDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Blood Grunt"));
		tmppool.push_back(CARD("Brood Walker"));
		tmppool.push_back(CARD("Brood Walker"));
		tmppool.push_back(CARD("Blood Spout"));
		tmppool.push_back(CARD("Blood Rider"));
		tmppool.push_back(CARD("Venomous Raptor"));
		tmppool.push_back(CARD("Venomous Raptor"));
		tmppool.push_back(CARD("Polluted Blaster"));
		tmppool.push_back(CARD("Desolate Sage"));

		PickACard(tmppool,TartarusSwarmDeck);
		PickACard(tmppool,TartarusSwarmDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Ravager"));
		tmppool.push_back(CARD("Sarcous Grub"));
		tmppool.push_back(CARD("Feral Scorpion"));
		tmppool.push_back(CARD("Feral Scorpion"));
		tmppool.push_back(CARD("Acid Spewer"));
		tmppool.push_back(CARD("Mawcor"));
		tmppool.push_back(CARD("Mawcor"));
		tmppool.push_back(CARD("Desolate Sage"));

		PickACard(tmppool,TartarusSwarmDeck);
		PickACard(tmppool,TartarusSwarmDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Desolate Sage"));
		tmppool.push_back(&CDB[236]); // carrion retriever
		tmppool.push_back(CARD("Feral Scorpion"));
		tmppool.push_back(CARD("Feral Scorpion"));
		tmppool.push_back(CARD("Mawcor"));
		tmppool.push_back(CARD("Blight Crusher"));
		tmppool.push_back(CARD("Blight Crusher"));
		tmppool.push_back(&CDB[3013]); // acid splash

		PickACard(tmppool,TartarusSwarmDeck);
		PickACard(tmppool,TartarusSwarmDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Destructive Behemoth"));
		tmppool.push_back(CARD("Draconian Queen"));
		tmppool.push_back(CARD("Draconian Queen"));
		tmppool.push_back(CARD("Pummeller"));
		tmppool.push_back(CARD("Nightmare"));
		tmppool.push_back(CARD("Diesel"));

		PickACard(tmppool,TartarusSwarmDeck);

		tmppool.clear();
		tmppool.push_back(&CDB[3013]); // acid splash
		tmppool.push_back(CARD("Irradiation"));
		tmppool.push_back(CARD("Full Power"));
		tmppool.push_back(CARD("Asylum"));
		tmppool.push_back(CARD("Asylum"));
		tmppool.push_back(CARD("Blood Pool"));
		tmppool.push_back(CARD("Blight Tower"));

		PickACard(tmppool,TartarusSwarmDeck);
		PickACard(tmppool,TartarusSwarmDeck);

		tmppool.clear(); // cleanup
	}
	void GenBehemothDeck(VCARDS &BehemothDeck)
	{
		// Gen Behemoth deck
		BehemothDeck.push_back(CARD("Smash"));
		BehemothDeck.push_back(CARD("Fireball"));
		BehemothDeck.push_back(CARD("Carapace Quake"));
		BehemothDeck.push_back(CARD("Behemoth Pup"));
		BehemothDeck.push_back(CARD("Wild Leeches"));
		BehemothDeck.push_back(CARD("Bountiful Tick"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Bountiful Tick"));
		tmppool.push_back(CARD("Carcass Scrounge"));
		tmppool.push_back(CARD("Groteske"));
		tmppool.push_back(CARD("Groteske"));
		tmppool.push_back(CARD("Hatchet"));
		tmppool.push_back(CARD("Hatchet"));

		PickACard(tmppool,BehemothDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Wild Leeches"));
		tmppool.push_back(CARD("Lummox"));
		tmppool.push_back(CARD("Tartarus Anomaly"));
		tmppool.push_back(CARD("Polluted Blaster"));
		tmppool.push_back(CARD("Desolate Sage"));
		tmppool.push_back(CARD("Desolate Sage"));
		tmppool.push_back(CARD("Slasher"));
		tmppool.push_back(CARD("Festerich"));
		tmppool.push_back(CARD("Festerich"));

		PickACard(tmppool,BehemothDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Desolate Sage"));
		tmppool.push_back(CARD("Behemoth Pup"));
		tmppool.push_back(CARD("Feral Scorpion"));
		tmppool.push_back(CARD("Mawcor"));
		tmppool.push_back(CARD("Mawcor"));
		tmppool.push_back(CARD("Flesheater"));
		tmppool.push_back(CARD("Pouncer"));
		tmppool.push_back(CARD("Impaler"));
		tmppool.push_back(CARD("Carrion Retriever"));
		tmppool.push_back(CARD("Carrion Retriever"));

		PickACard(tmppool,BehemothDeck);
		PickACard(tmppool,BehemothDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Pummeller"));
		tmppool.push_back(CARD("Pummeller"));
		tmppool.push_back(CARD("Abomination"));
		tmppool.push_back(CARD("Abomination"));
		tmppool.push_back(CARD("Gore Crawler"));
		tmppool.push_back(CARD("Draconian Queen"));

		PickACard(tmppool,BehemothDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Smash"));
		tmppool.push_back(CARD("Fireball"));
		tmppool.push_back(CARD("Demolish"));
		tmppool.push_back(CARD("Death from Above"));
		tmppool.push_back(CARD("Chaos Wave"));
		tmppool.push_back(CARD("Chaos Wave"));
		tmppool.push_back(CARD("Maximum Damage"));
		tmppool.push_back(CARD("Destruction"));
		tmppool.push_back(CARD("Smash and Grab"));

		PickACard(tmppool,BehemothDeck);
		PickACard(tmppool,BehemothDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Carapace Quake"));
		tmppool.push_back(CARD("Plague"));
		tmppool.push_back(CARD("Plague"));
		tmppool.push_back(CARD("Impede Assault"));
		tmppool.push_back(CARD("Irradiation"));
		tmppool.push_back(CARD("Smash and Grab"));
		tmppool.push_back(CARD("Massive Assault"));
		tmppool.push_back(CARD("Pouncer"));

		PickACard(tmppool,BehemothDeck);
		PickACard(tmppool,BehemothDeck);

		tmppool.clear(); // cleanup
	}
	void GenMiasmaDeck(VCARDS &MiasmaDeck)
	{
		// Gen Miasma deck
		MiasmaDeck.push_back(CARD("Shadow"));
		MiasmaDeck.push_back(&CDB[390]); // Bloodsucker
		MiasmaDeck.push_back(CARD("Mist Inhaler"));
		MiasmaDeck.push_back(&CDB[2075]); // Haze Geyser
		MiasmaDeck.push_back(CARD("Tartarus Reaper"));
		MiasmaDeck.push_back(CARD("Crazed Assailant"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Hatchet"));
		tmppool.push_back(CARD("Hatchet"));
		tmppool.push_back(CARD("Shapeshifter"));
		tmppool.push_back(CARD("Intruder"));
		tmppool.push_back(CARD("Gruesome Crawler"));
		tmppool.push_back(CARD("Gruesome Crawler"));

		PickACard(tmppool,MiasmaDeck);
		PickACard(tmppool,MiasmaDeck);

		tmppool.clear();
		tmppool.push_back(&CDB[390]); // Bloodsucker
		tmppool.push_back(&CDB[2075]); // Haze Geyser
		tmppool.push_back(CARD("Locust Swarm"));
		tmppool.push_back(CARD("Brood Walker"));
		tmppool.push_back(CARD("Brood Walker"));
		tmppool.push_back(CARD("Blood Spout"));
		tmppool.push_back(CARD("Blood Spout"));
		tmppool.push_back(CARD("Venomous Raptor"));
		tmppool.push_back(CARD("Mad Cur"));
		tmppool.push_back(CARD("Infectious Stalker"));
		tmppool.push_back(CARD("Infectious Stalker"));
		tmppool.push_back(CARD("Redeemer"));
		tmppool.push_back(CARD("Tartarus Anomaly"));
		tmppool.push_back(CARD("Tartarus Anomaly"));
		tmppool.push_back(CARD("Desolate Sage"));
		tmppool.push_back(CARD("Festerich"));

		PickACard(tmppool,MiasmaDeck);
		PickACard(tmppool,MiasmaDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Shadow"));
		tmppool.push_back(CARD("Crazed Assailant"));

		tmppool.push_back(CARD("Sarcous Grub"));
		tmppool.push_back(CARD("Blight Crusher"));
		tmppool.push_back(CARD("Mawcor"));
		tmppool.push_back(CARD("Feral Scorpion"));
		tmppool.push_back(CARD("Pouncer"));
		tmppool.push_back(CARD("Pouncer"));
		tmppool.push_back(CARD("Impaler"));
		tmppool.push_back(CARD("Impaler"));
		tmppool.push_back(CARD("Incinerator"));

		PickACard(tmppool,MiasmaDeck);
		PickACard(tmppool,MiasmaDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Mist Inhaler"));
		tmppool.push_back(CARD("Abomination"));
		tmppool.push_back(CARD("Draconian Queen"));
		tmppool.push_back(CARD("Draconian Queen"));
		tmppool.push_back(CARD("Pummeller"));
		tmppool.push_back(CARD("Pummeller"));
		tmppool.push_back(CARD("Mass Infester"));
		tmppool.push_back(CARD("Blight Golem"));
		tmppool.push_back(CARD("Raging Prowler"));
		tmppool.push_back(CARD("Diesel"));
		tmppool.push_back(CARD("Diesel"));
		tmppool.push_back(CARD("Nightmare"));
		tmppool.push_back(CARD("Lumbering Ogre"));
		tmppool.push_back(CARD("Plague Wyrm"));
		tmppool.push_back(CARD("Plague Wyrm"));

		PickACard(tmppool,MiasmaDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Blood Pool"));
		tmppool.push_back(CARD("Blood Pool"));
		tmppool.push_back(&CDB[2075]); // Haze Geyser
		tmppool.push_back(CARD("Pandemic"));
		tmppool.push_back(CARD("Sulfuris Acid Pit"));
		tmppool.push_back(CARD("Blight Tower"));
		tmppool.push_back(CARD("Fulfilling Myst"));
		tmppool.push_back(CARD("Fulfilling Myst"));
		tmppool.push_back(CARD("Plague"));
		tmppool.push_back(CARD("Exhaust"));
		tmppool.push_back(CARD("Full Power"));
		tmppool.push_back(CARD("Festering Wasteland"));

		PickACard(tmppool,MiasmaDeck);
		PickACard(tmppool,MiasmaDeck);

		tmppool.clear(); // cleanup
	}
	void GenBlightbloomDeck(VCARDS &BlightbloomDeck)
	{
		// Gen Blightbloom deck
		BlightbloomDeck.push_back(CARD("Bloom"));
		BlightbloomDeck.push_back(CARD("Corrosive Spores")); // Bloodsucker
		BlightbloomDeck.push_back(CARD("Thrashing Barbel"));
		BlightbloomDeck.push_back(CARD("Rejuvenator"));
		BlightbloomDeck.push_back(CARD("Sunder"));
		BlightbloomDeck.push_back(CARD("Sporscism"));
		BlightbloomDeck.push_back(CARD("Infected Spire"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Bloom"));
		tmppool.push_back(CARD("Sporscism"));
		tmppool.push_back(CARD("Annelid Mass"));
		tmppool.push_back(CARD("Moloch"));
		tmppool.push_back(CARD("Shapeshifter"));
		tmppool.push_back(CARD("Shapeshifter"));
		tmppool.push_back(CARD("Carcass Scrounge"));
		tmppool.push_back(CARD("Crippler"));
		tmppool.push_back(CARD("Corrosive Fiend"));
		tmppool.push_back(CARD("Whiplash"));
		tmppool.push_back(CARD("Whiplash"));
		tmppool.push_back(CARD("Gruesome Crawler"));
		tmppool.push_back(CARD("Gruesome Crawler"));
		tmppool.push_back(CARD("Gruesome Crawler"));

		PickACard(tmppool,BlightbloomDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Brood Walker"));
		tmppool.push_back(CARD("Blood Spout"));
		tmppool.push_back(CARD("Blood Spout"));
		tmppool.push_back(CARD("Infectious Stalker"));
		tmppool.push_back(CARD("Redeemer"));
		tmppool.push_back(CARD("Tartarus Anomaly"));
		tmppool.push_back(CARD("Festerich"));
		tmppool.push_back(CARD("Festerich"));

		PickACard(tmppool,BlightbloomDeck);
		PickACard(tmppool,BlightbloomDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Infected Spire"));
		tmppool.push_back(CARD("Sarcous Grub"));
		tmppool.push_back(CARD("Impaler"));
		tmppool.push_back(CARD("Flesheater"));
		tmppool.push_back(CARD("Flesheater"));
		tmppool.push_back(CARD("Pouncer"));
		tmppool.push_back(CARD("Pouncer"));
		tmppool.push_back(CARD("Carrion Retriever"));

		PickACard(tmppool,BlightbloomDeck);
		PickACard(tmppool,BlightbloomDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Banshee"));
		tmppool.push_back(CARD("Draconian Queen"));
		tmppool.push_back(CARD("Draconian Queen"));
		tmppool.push_back(CARD("Pummeller"));
		tmppool.push_back(CARD("Pummeller"));
		tmppool.push_back(CARD("Lumbering Ogre"));
		tmppool.push_back(CARD("Plague Wyrm"));

		PickACard(tmppool,BlightbloomDeck);
		PickACard(tmppool,BlightbloomDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Blood Pool"));
		tmppool.push_back(CARD("Tartarus Pits"));
		tmppool.push_back(CARD("Sulfuris Acid Pit"));
		tmppool.push_back(CARD("Fulfilling Myst"));
		tmppool.push_back(CARD("Sunder"));
		tmppool.push_back(CARD("Impaler"));
		tmppool.push_back(CARD("Redeemer"));

		PickACard(tmppool,BlightbloomDeck);

		tmppool.clear(); // cleanup
	}
	void GenGoreTyphonDeck(VCARDS &GoreTyphonDeck)
	{
		// Gen Gore Typhon deck
		GoreTyphonDeck.push_back(CARD("Typhon's Grasp"));
		GoreTyphonDeck.push_back(CARD("Blight Claw")); // Bloodsucker
		GoreTyphonDeck.push_back(CARD("Loyal Legion"));
		GoreTyphonDeck.push_back(CARD("Loyal Legion"));
		GoreTyphonDeck.push_back(CARD("Discharge Cannon"));
		GoreTyphonDeck.push_back(CARD("Typhon's Beast"));
		GoreTyphonDeck.push_back(CARD("Impale"));

		VCARDS tmppool;
		tmppool.push_back(CARD("Loyal Legion"));
		tmppool.push_back(CARD("Blood Grunt"));
		tmppool.push_back(CARD("Blood Grunt"));
		tmppool.push_back(CARD("Hatchet"));
		tmppool.push_back(CARD("Hatchet"));
		tmppool.push_back(CARD("Heartripper"));
		tmppool.push_back(CARD("Groteske"));
		tmppool.push_back(CARD("Gruesome Crawler"));
		tmppool.push_back(CARD("Hunter"));
		tmppool.push_back(CARD("Lodestar"));

		PickACard(tmppool,GoreTyphonDeck);
		PickACard(tmppool,GoreTyphonDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Venomous Raptor"));
		tmppool.push_back(CARD("Venomous Raptor"));
		tmppool.push_back(CARD("Redeemer"));
		tmppool.push_back(CARD("Desolate Sage"));
		tmppool.push_back(CARD("Desolate Sage"));
		tmppool.push_back(CARD("Festerich"));
		tmppool.push_back(CARD("Havoc"));
		tmppool.push_back(CARD("Havoc"));
		tmppool.push_back(CARD("Mech Walker"));
		tmppool.push_back(CARD("Scorpion"));
		tmppool.push_back(CARD("Lodestar"));

		PickACard(tmppool,GoreTyphonDeck);
		PickACard(tmppool,GoreTyphonDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Typhon's Beast"));
		tmppool.push_back(CARD("Feral Scorpion"));
		tmppool.push_back(CARD("Blight Crusher"));
		tmppool.push_back(CARD("Blight Crusher"));
		tmppool.push_back(CARD("Mawcor"));
		tmppool.push_back(CARD("Obsidian"));
		tmppool.push_back(CARD("Dozer Tank"));
		tmppool.push_back(CARD("Dozer Tank"));
		tmppool.push_back(CARD("Colossus"));
		tmppool.push_back(CARD("Colossus"));
		tmppool.push_back(CARD("Jericho"));
		tmppool.push_back(CARD("Howitzer"));
		tmppool.push_back(CARD("Demolition Bot"));
		tmppool.push_back(CARD("Toxic Cannon"));

		PickACard(tmppool,GoreTyphonDeck);
		PickACard(tmppool,GoreTyphonDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Nightmare"));
		tmppool.push_back(CARD("Diesel"));
		tmppool.push_back(CARD("Diesel"));
		tmppool.push_back(CARD("Plague Wyrm"));
		tmppool.push_back(CARD("Gun Raven"));
		tmppool.push_back(CARD("Gargantuan"));
		tmppool.push_back(CARD("Bolide Walker"));
		tmppool.push_back(CARD("Razor"));

		PickACard(tmppool,GoreTyphonDeck);

		tmppool.clear();
		tmppool.push_back(CARD("Impale"));
		tmppool.push_back(CARD("Electromagnetic Pulse"));
		tmppool.push_back(CARD("Irradiation"));
		tmppool.push_back(CARD("Chaos Wave"));
		tmppool.push_back(CARD("Maximum Damage"));
		tmppool.push_back(CARD("Plague"));
		tmppool.push_back(CARD("Destruction"));
		tmppool.push_back(CARD("Gatling Tower"));
		tmppool.push_back(CARD("Acid Splash"));
		tmppool.push_back(CARD("Missile Silo"));
		tmppool.push_back(CARD("Blood Wall"));

		PickACard(tmppool,GoreTyphonDeck);

		tmppool.clear(); // cleanup
	}*/
};