// dllmain.cpp : Defines the entry point for the DLL application.
#include "..\EXE\deck.h"
#include "..\EXE\cards.h"

#define IDAPI   __declspec(dllexport)


#include <windows.h>
#include "process.h"

CardDB DB;
ActiveDeck gAtk;
ActiveDeck gDef;
MUUINT MyCards;
/*
struct RESULTS
{
	DWORD Win;
	DWORD Loss;
	DWORD Points;
	DWORD AutoPoints;
	DWORD LPoints;
	DWORD LAutoPoints;
	RESULTS()
	{
		Win = 0;
		Loss = 0;
		Points = 0;
		AutoPoints = 0;
		LPoints = 0;
		LAutoPoints = 0;
	}
	void Add(const RESULTS rAdd)
	{
		Win += rAdd.Win;
		Loss += rAdd.Loss;
		Points += rAdd.Points;
		AutoPoints += rAdd.AutoPoints;
		LPoints += rAdd.LPoints;
		LAutoPoints += rAdd.LAutoPoints;
	}
};
*/
void Simulate(ActiveDeck &tAtk, ActiveDeck &tDef, RESULTS &r, bool bSurge = false)
{
/*
Maximum of 25 points per player. 
+10 points for winning. 
+5 points for killing the enemy commander. 
+5 points for winning by turn 10 
+X points for dealing X damage to the commander until the exact moment he dies (max 10)? <— tweak 
+5 points for destroying all enemy Assault and Structure cards
Automatic:
+10 points for winning. 
+1 point for each damage dealt to the enemy commander, up to 10. (Siphon does not reduce this.) 
+5 points for winning by turn 10
IF you surge (ie, defense goes first) and you win then you get +20 points with or without auto .
*/
	for (UCHAR i=0; (i < 50); i++)
	{
		if (bSurge)
		{
			tDef.AttackDeck(tAtk);
			if (!tAtk.Commander.IsAlive())
			{
				r.LPoints+=10; // for winning 
				r.LAutoPoints+=10;
				r.LPoints+=5; // dying
				r.Loss++;
				if (i < 10)
				{
					r.LPoints+=5; // +5 points for losing by turn 10 
					r.LAutoPoints+=5; // +5 points for losing by turn 10 
				}
				break;
			}
		}
		tAtk.AttackDeck(tDef);
		if (!tDef.Commander.IsAlive())
		{
			r.Win++;
			r.Points+=10; // +10 points for winning 
			r.AutoPoints+=10;
			r.Points+=5; // +5 points for killing the enemy commander
			if (bSurge)
			{
				r.Points+=20; // +20 points for winning on surge
				r.AutoPoints+=20;
			}
			if (i < 10)
			{
				r.Points+=5; // +5 points for winning by turn 10 
				r.AutoPoints+=5; // +5 points for winning by turn 10 
			}
			break;
		}
		if (!bSurge)
		{
			tDef.AttackDeck(tAtk);
			if (!tAtk.Commander.IsAlive())
			{
				r.LPoints+=10; // for winning 
				r.LAutoPoints+=10;
				r.LPoints+=5; // dying
				r.Loss++;
				if (i < 10)
				{
					r.LPoints+=5; // +5 points for losing by turn 10 
					r.LAutoPoints+=5; // +5 points for losing by turn 10 
				}
				break;
			}
		}
	}
	if (tDef.Commander.IsDefined())
	{
		if (tDef.Commander.GetOriginalCard()->GetHealth() > 10)
		{
			r.Points+=10; // +X points for dealing X damage to the commander
			r.AutoPoints+=10; // +X points for dealing X damage to the commander
		}
		else
		{
			r.Points+=tDef.Commander.GetOriginalCard()->GetHealth();
			r.AutoPoints+=tDef.Commander.GetOriginalCard()->GetHealth();
		}
	}
	if (tAtk.Commander.IsDefined())
	{
		if (tAtk.Commander.GetOriginalCard()->GetHealth() > 10)
		{
			r.LPoints+=10; // +X points for dealing X damage to the commander
			r.LAutoPoints+=10; // +X points for dealing X damage to the commander
		}
		else
		{
			r.LPoints+=tAtk.Commander.GetOriginalCard()->GetHealth();
			r.LAutoPoints+=tAtk.Commander.GetOriginalCard()->GetHealth();
		}
	}
	if (tDef.Deck.empty()) // +5 points for destroying all enemy Assault and Structure cards
		r.Points+=5;
	if (tAtk.Deck.empty()) // +5 points for losing all your Assault and Structure cards
		r.LPoints+=5;
}

struct EVAL_THREAD_PARAMS
{
	ActiveDeck *Atk;
	ActiveDeck *Def;
	DWORD gamesperthread;
	DWORD Seed;
	RESULTS r;
	bool bSurge;
};
           
static unsigned int __stdcall ThreadFunc(void *pvParam)
{
	EVAL_THREAD_PARAMS *p = (EVAL_THREAD_PARAMS *)pvParam;
	srand((unsigned)p->Seed); // it seems like each thread shares seed with others before it starts, so we should reset seed or we will gate the same random sequences in all threads
	for (DWORD i=0;i<p->gamesperthread;i++)
	{
		ActiveDeck Atk(*p->Atk);
		ActiveDeck Def(*p->Def);
		Simulate(Atk,Def,p->r,p->bSurge);
	}
	//_endthread();
	return (UINT)p;
}

extern "C" 
{
	IDAPI UINT GetDBSize()
	{
		return DB.GetSize();
	}
	IDAPI bool LoadCardXML(const char *FileName, char *returnnewcards, size_t MaxBufferSize)
	{
		return DB.LoadCardXML(FileName,returnnewcards,MaxBufferSize);
	}
	IDAPI bool LoadMissionXML(const char *FileName)
	{
		return DB.LoadMissionXML(FileName);
	}
	IDAPI bool LoadAchievementXML(const char *FileName)
	{
		return DB.LoadAchievementXML(FileName);
	}
	IDAPI bool LoadRaidXML(const char *FileName)
	{
		return DB.LoadRaidXML(FileName);
	}
	IDAPI bool LoadOwnedCards(const char *FileName)
	{
		MyCards.clear();
		return DB.LoadOwnedCards(FileName,MyCards);
	}
	IDAPI UINT CheckOwnedCard(const UINT Id)
	{
		MUUINT::iterator mi = MyCards.find(Id);
		if (mi == MyCards.end())
			return 0;
		return mi->second;
	}
	IDAPI DWORD LoadPointers(Card **Ptr, DWORD MaxCount)
	{
		return DB.LoadPointers(Ptr,MaxCount);
	}
	IDAPI const Card* GetCard(const char *Name) 
	{		
		return DB.GetCard(Name);
	};
	IDAPI const Card* GetCardByID(const UINT Id) 
	{		
		return &DB.GetCard(Id);
	};
	IDAPI const bool GetCardListSorted(char *buffer, DWORD size)
	{
		return DB.GetCardListSorted(buffer, size);
	};
	IDAPI const char * GetCustomDecksList(int Tag, char *buffer, DWORD size)
	{
		return DB.GetCustomDecksList(buffer, size, Tag);
	};
	IDAPI const char * GetMissionDecksList(char *buffer, DWORD size, bool bSortById = true)
	{
		return DB.GetMissionDecksList(buffer, size, bSortById);
	};
	IDAPI const char* GetCustomDeck(const char* DeckName, int Tag, char *buffer, DWORD size)
	{
		return DB.GetCustomDeck(DeckName, Tag, buffer, size);
	};
	IDAPI const char* GetMissionDeck(const char* DeckName, char *buffer, DWORD size)
	{
		return DB.GetMissionDeck(DeckName, buffer, size);
	};
	IDAPI const UINT GetMissionDeckIndex(const char* DeckName)
	{
		return DB.GetMissionDeckIndex(DeckName);
	};
	IDAPI const char * GetRaidDecksList(char *buffer, DWORD size)
	{
		return DB.GetRaidDecksList(buffer, size);
	};
	IDAPI const UINT GetRaidCommanderID(UINT RaidIndex)
	{
		return DB.GetRaidCommanderID(RaidIndex);
	};
	IDAPI int LoadCustomDecks(const char* FileName)
	{
		return DB.LoadDecks(FileName,false);
	}
	IDAPI bool ClearCustomDecks()
	{
		return DB.ClearDeckIndex();
	}
	IDAPI bool AbilityHasExtendedDesc(UCHAR AbilityID)
	{
		return 
			(AbilityID == ACTIVATION_ENFEEBLE) ||
			(AbilityID == ACTIVATION_HEAL) ||
			(AbilityID == ACTIVATION_PROTECT) ||			
			(AbilityID == ACTIVATION_RALLY) ||
			(AbilityID == ACTIVATION_REPAIR) ||
			(AbilityID == ACTIVATION_SHOCK) ||
			(AbilityID == ACTIVATION_SIEGE) ||
			(AbilityID == ACTIVATION_STRIKE) ||
			(AbilityID == ACTIVATION_SUPPLY) ||
			(AbilityID == ACTIVATION_WEAKEN) ||
			(AbilityID == DEFENSIVE_ARMORED) ||
			(AbilityID == DEFENSIVE_COUNTER) ||
			(AbilityID == DEFENSIVE_REGENERATE) ||
			(AbilityID == COMBAT_ANTIAIR) ||
			(AbilityID == COMBAT_BURST) ||
			(AbilityID == COMBAT_FLURRY) ||
			(AbilityID == COMBAT_PIERCE) ||
			(AbilityID == COMBAT_VALOR) ||
			(AbilityID == DMGDEPENDANT_BERSERK) ||
			(AbilityID == DMGDEPENDANT_CRUSH) ||
			(AbilityID == DMGDEPENDANT_LEECH) ||
			(AbilityID == DMGDEPENDANT_POISON) ||
			(AbilityID == DMGDEPENDANT_SIPHON) ||
			(AbilityID == SPECIAL_BACKFIRE);
	}
	IDAPI DWORD GetSets(UINT *Indexes,CardSet *Sets, DWORD Size)
	{
		DWORD cnt = 0;
		for (MSETS::iterator mi = DB.SetIndex.begin(); (mi != DB.SetIndex.end()) && (cnt < Size); mi++)
		{
			Indexes[cnt] = mi->first;
			Sets[cnt] = mi->second;
			cnt++;
		}
		return cnt;
	};
	// this is obsolete
	IDAPI DWORD Evaluate(const char *AtkDeck, const char* DefDeck, DWORD gamescount)
	{
		ActiveDeck Atk;
		DB.CreateDeck(AtkDeck,Atk);
		ActiveDeck Def;
		DB.CreateDeck(DefDeck,Def);
		DWORD wins = 0;
		for (DWORD k=0;k<gamescount;k++)
		{
			ActiveDeck tAtk(Atk);
			ActiveDeck tDef(Def);
			for (UCHAR i=0; (i < 50); i++)
			{
				tAtk.AttackDeck(tDef);
				if (!tDef.Commander.IsAlive())
				{
					wins++;
					break;
				}
				tDef.AttackDeck(tAtk);
				if (!tAtk.Commander.IsAlive())
				{
					//r.Loss++;
					break;
				}
			}
		}
		return wins;
	}
	IDAPI void PrepareDecks(const char *AtkDeck, const char* DefDeck)
	{
		gAtk.Deck.clear();
		gDef.Deck.clear();
		DB.CreateDeck(AtkDeck,gAtk);
		DB.CreateDeck(DefDeck,gDef);
	}
	IDAPI void PrepareDeck(const char *AtkDeck)
	{
		gAtk.Deck.clear();
		DB.CreateDeck(AtkDeck,gAtk);
	}
	IDAPI void EvaluateOnce(RESULTS &r, bool bSurge=false)
	{
		ActiveDeck tAtk(gAtk);
		ActiveDeck tDef(gDef);
		Simulate(tAtk,tDef,r,bSurge);
	}
	IDAPI void EvaluateInThreads(DWORD Seed, RESULTS &ret, DWORD gamesperthread, DWORD threadscount = 1, bool bSurge = false)
	{
		if (threadscount <= 1)
		{
			srand((unsigned)Seed);
			for (DWORD i=0;i<gamesperthread;i++)
			{
				ActiveDeck tAtk(gAtk);
				ActiveDeck tDef(gDef);
				Simulate(tAtk,tDef,ret,bSurge);
			}
		}
		else
		{
		#define MAX_THREADS	50
			unsigned int  m_uiThreadID[MAX_THREADS];
			unsigned long m_ulThreadHandle[MAX_THREADS];
			EVAL_THREAD_PARAMS parms[MAX_THREADS]; 
			if (threadscount > MAX_THREADS)
				threadscount = MAX_THREADS;
			for (DWORD i=0;i<threadscount;i++)
			{
				parms[i].Atk = &gAtk;
				parms[i].Def = &gDef;
				parms[i].gamesperthread = gamesperthread;
				parms[i].Seed = Seed + i; // offset seed or we will have same results for all threads
				parms[i].bSurge = bSurge;

				m_ulThreadHandle[i] = _beginthreadex(0,
											0,
											ThreadFunc,
											&parms,
											0,
											&m_uiThreadID[i]);
				// if(!m_ulThreadHandle)
					// Could not create thread
			}
			for (DWORD i=0;i<threadscount;i++)
			{
				cwait(NULL,m_ulThreadHandle[i],NULL); // now wait them all, order doesn't matter since we will have to wait all of them
				// collect results
				ret.Add(parms[i].r);
			}
		}
	}
	IDAPI void SpeedTest() // just a check, should take 12-16 sec in exe
	{
		ActiveDeck X(DB.CARD("Thadius"));
		X.Deck.push_back(DB.CARD("Sharpshooter"));
		X.Deck.push_back(DB.CARD("Sharpshooter"));
		X.Deck.push_back(DB.CARD("Sharpshooter"));
		X.Deck.push_back(DB.CARD("Sharpshooter"));
		X.Deck.push_back(DB.CARD("Sharpshooter"));
		X.Deck.push_back(DB.CARD("Sharpshooter"));
		X.Deck.push_back(DB.CARD("Sharpshooter"));
		X.Deck.push_back(DB.CARD("Sharpshooter"));
		X.Deck.push_back(DB.CARD("Sharpshooter"));
		X.Deck.push_back(DB.CARD("Rocket Infantry"));
		ActiveDeck Y(DB.CARD("Vyander"));
		Y.Deck.push_back(DB.CARD("Vaporwing"));
		Y.Deck.push_back(DB.CARD("Vaporwing"));
		Y.Deck.push_back(DB.CARD("Vaporwing"));
		Y.Deck.push_back(DB.CARD("Vaporwing"));
		Y.Deck.push_back(DB.CARD("Vaporwing"));
		Y.Deck.push_back(DB.CARD("Vaporwing"));
		Y.Deck.push_back(DB.CARD("Vaporwing"));
		Y.Deck.push_back(DB.CARD("Vaporwing"));
		Y.Deck.push_back(DB.CARD("Vaporwing"));
		Y.Deck.push_back(DB.CARD("Vaporwing"));
		gAtk = ActiveDeck(X);
		gDef = ActiveDeck(Y);
		RESULTS ret;
		EvaluateInThreads(100500,ret,250000,4,false);
	}		
	IDAPI void EvaluateRaidOnce(RESULTS &r, DWORD RaidID)
	{
		ActiveDeck tAtk(gAtk);
		ActiveDeck tDef;
		DB.GenRaidDeck(tDef,RaidID);
					
		for (UCHAR i=0; (i < 50); i++)
		{
			tAtk.AttackDeck(tDef);
			if (!tDef.Commander.IsAlive())
			{
				r.Win++;
				r.Points+=10; // +10 points for winning 
				r.AutoPoints+=10;
				r.Points+=5; // +5 points for killing the enemy commander
				if (i < 10)
				{
					r.Points+=5; // +5 points for winning by turn 10 
					r.AutoPoints+=5; // +5 points for winning by turn 10 
				}
				if (tDef.Deck.empty()) // +5 points for destroying all enemy Assault and Structure cards
					r.Points+=5;
				break;
			}
			tDef.AttackDeck(tAtk);
			if (!tAtk.Commander.IsAlive())
			{
				r.Loss++;
				break;
			}
		}
	}
	IDAPI void GetSkillList(char *Buffer, DWORD MaxBufferSize)
	{
		Buffer[0]=0;
		for (UCHAR i=0;i<CARD_ABILITIES_MAX;i++)
		{
			if (i > 0)
				strcat_s(Buffer,MaxBufferSize,",");
			strcat_s(Buffer,MaxBufferSize,DB.GetSkill(i));
		}
	}
	IDAPI void GetAchievementList(char *Buffer, DWORD MaxBufferSize)
	{
		return DB.GetAchievementList(Buffer,MaxBufferSize);
	}
	IDAPI const char *GetAchievementDescription(UINT AchievementIndex)
	{
		return DB.GetAchievementDescription(AchievementIndex);
	}
	IDAPI UINT GetAchievementID(UINT AchievementIndex)
	{
		return DB.GetAchievementID(AchievementIndex);
	}
	IDAPI bool CheckAchievementMission(UINT AchievementIndex, UINT MissionID)
	{
		return DB.CheckAchievementMission(AchievementIndex,MissionID);
	}	
	IDAPI const char *GetSkillName(DWORD Id)
	{
		return DB.Skills[Id].SkillName;
	}
	IDAPI UCHAR GetSkillID(const char *Skill)
	{
		return DB.GetSkillID(Skill);
	}
	IDAPI size_t GetCardSize()
	{
		return sizeof(Card);
	}
	IDAPI bool InsertCustomDeck(const char *List)
	{
		return DB.InsertDeck(0,List);
	}
	IDAPI bool GetHashFromDeck(const char *Deck, char *Buffer, DWORD MaxBufferSize)
	{
		ActiveDeck D;
		if (!DB.CreateDeck(Deck,D))
			return false;
		strcpy_s(Buffer,MaxBufferSize,D.GetHash64().c_str());
		return true;
	}
	IDAPI bool GetDeckFromHash(const char *Hash, char *Buffer, DWORD MaxBufferSize)
	{
		ActiveDeck D(Hash,DB.GetPointer());
		if ((!D.Commander.IsDefined()) && D.Deck.empty())
			return false;
		strcpy_s(Buffer,MaxBufferSize,D.GetDeck().c_str());
		return true;
	}
	IDAPI bool GetDeckFromString(const char *CardList, char *OutputIDList)
	{
		return DB.InsertDeck(0,CardList,OutputIDList);
	}
	IDAPI bool RateCard(const UINT Id, double &OffenceValue, double &DefenceValue, const UCHAR iFormulaUsed = 0)
	{
		return DB.RateCard(Id, OffenceValue, DefenceValue, iFormulaUsed);
	}
	IDAPI bool BuildResultHash(const char *CardList, UINT Version, UINT Revision, UINT HashType, UINT HashID, UINT MissionID, UINT GamesOverall, UINT GamesWon, char *Buffer, DWORD MaxBufferSize)
	{
#define HASH_VERSION	1
		ActiveDeck X;
		DB.CreateDeck(CardList,X);
		if (X.Deck.empty() || (!X.IsValid()))
			return false;

		UINT DeckChecksum = X.Commander.GetId();
		for (VCARDS::iterator vi = X.Deck.begin();vi!=X.Deck.end();vi++)
			DeckChecksum += vi->GetId();
/*
Everything is in base64
--------- Hash Version 0 ---------
H - hash version (0 for now)
V - EvaluateDecks version
RR - EvaluateDecks revision
T - hash type (0 - mission(auto), 1 - mission(manual), 2 - raid(auto), 3 - raid(manual), 4 - achievement(auto), 5 - achievement(manual), 6+ - custom deck?)
II - mission/raid/achievement id
GGGG - count of games overall
WWWW - count of games won
D - deck checksum ((sum of deck card IDs) mod 64)
R - result checksum ((H + V + RR + T + II + GGGG + WWWW) mod 64)

Full result, containing both deck definition and result of evaluations may look like this:
DECKHASH.HVRRTIIGGGGWWWWDR

--------- Hash Version 1 ---------
H - hash version (1)
V - EvaluateDecks version
RR - EvaluateDecks revision
T - hash type (0 - mission(auto), 1 - mission(manual), 2 - raid(auto), 3 - raid(manual), 4 - achievement(auto), 5 - achievement(manual), 6+ - custom deck?)
II - mission/raid/achievement id
MM - mission id(for missions it is the same as II, 0 for raids, makes sense only for achievements)
GGGG - count of games overall
WWWW - count of games won
D - deck checksum ((sum of deck card IDs) mod 64)
R - result checksum ((H + V + RR + T + II + MM + GGGG + WWWW) mod 64)

Full result, containing both deck definition and result of evaluations may look like this:
DECKHASH.HVRRTIIMMGGGGWWWWDR
*/
		string s = X.GetHash64();
		s += ".";
		unsigned short c = ID2BASE64(HASH_VERSION);
		s += ((char*)&c + 1); // need only last 1 char
		c = ID2BASE64(Version);
		s += ((char*)&c + 1); // need only last 1 char
		c = ID2BASE64(Revision);
		s += (char*)&c;
		c = ID2BASE64(HashType);
		s += ((char*)&c + 1); // need only last 1 char
		c = ID2BASE64(HashID);
		s += (char*)&c;
		c = ID2BASE64(MissionID);
		s += (char*)&c;
		c = ID2BASE64(GamesOverall / 4096);
		s += (char*)&c;
		c = ID2BASE64(GamesOverall % 4096);
		s += (char*)&c;
		c = ID2BASE64(GamesWon / 4096);
		s += (char*)&c;
		c = ID2BASE64(GamesWon % 4096);
		s += (char*)&c;
		c = ID2BASE64(DeckChecksum % 4096);
		s += ((char*)&c + 1); // need only last 1 char
		UINT ResultChecksum = HASH_VERSION + Version + Revision + HashType + HashID + MissionID + GamesOverall + GamesWon;
		c = ID2BASE64(ResultChecksum % 4096);
		s += ((char*)&c + 1); // need only last 1 char
		if (s.length() > MaxBufferSize)
			return false;
		strcpy(Buffer,s.c_str());		
		return true;
	}
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

