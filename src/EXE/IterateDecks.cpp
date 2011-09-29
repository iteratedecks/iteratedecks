// IterateDecks.cpp : Defines the entry point for the console application.
//

#include <tchar.h>

#include "stdio.h"
#include "time.h"

#include "deck.h"

#include "cards.h"

#include "branches.h"

#include "process.h"

#ifdef _DEBUG
#include <direct.h>
#endif

CardDB DB; // just to make all easier ...

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
	for (UCHAR i=0; (i < MAX_TURN); i++)
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
	const ActiveDeck *Atk;
	const ActiveDeck *Def;
	int RaidID;
	DWORD gamesperthread;
	DWORD Seed;
	RESULTS r;
	bool bSurge;
};

void EvaluateRaidOnce(const ActiveDeck gAtk, RESULTS &r, DWORD RaidID)
{
	ActiveDeck tAtk(gAtk);
	ActiveDeck tDef;
	DB.GenRaidDeck(tDef,RaidID);
					
	for (UCHAR i=0; (i < MAX_TURN); i++)
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

static unsigned int __stdcall ThreadFunc(void *pvParam)
{
	EVAL_THREAD_PARAMS *p = (EVAL_THREAD_PARAMS *)pvParam;
	srand((unsigned)p->Seed); // it seems like each thread shares seed with others before it starts, so we should reset seed or we will gate the same random sequences in all threads
	RESULTS lr;
	for (DWORD i=0;i<p->gamesperthread;i++)
	{
		if (p->RaidID < 0)
		{
			ActiveDeck Atk(*p->Atk);
			ActiveDeck Def(*p->Def);
			Simulate(Atk,Def,lr,p->bSurge);
		}
		else
			EvaluateRaidOnce(*(p->Atk),lr,(DWORD)p->RaidID);
	}
	//_endthread();
	p->r.Add(lr);
	return (UINT)p;
}

void EvaluateInThreads(DWORD Seed, const ActiveDeck &gAtk, const ActiveDeck &gDef, int RaidID, RESULTS &ret, DWORD gamesperthread, DWORD threadscount = 1, bool bSurge = false)
{
	if (threadscount <= 1)
	{
		srand((unsigned)Seed);
		for (DWORD i=0;i<gamesperthread;i++)
		{
			if (RaidID < 0)
			{
				ActiveDeck tAtk(gAtk);
				ActiveDeck tDef(gDef);
				Simulate(tAtk,tDef,ret,bSurge);
			}
			else
				EvaluateRaidOnce(gAtk,ret,(DWORD)RaidID);
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
			parms[i].RaidID = RaidID;
			parms[i].gamesperthread = gamesperthread;
			parms[i].Seed = Seed + i; // offset seed or we will have same results for all threads
			parms[i].bSurge = bSurge;

			m_ulThreadHandle[i] = _beginthreadex(0,
										0,
										ThreadFunc,
										&parms[i],
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

#include <windows.h>
//#include "Aclapi.h"

#define SM_NAME		"Local\\IterateDecksSharedMemory"
#define MAX_DECKSTRING_SIZE	1024
struct EVAL_PARAMS
{
	DWORD Seed;
	char AtkDeck[MAX_DECKSTRING_SIZE];
	char DefDeck[MAX_DECKSTRING_SIZE];
	DWORD GamesPerThread;
	DWORD Threads;
	RESULTS Result;
	DWORD Seconds;
	int RaidID;
	bool Surge;
};

int _tmain(int argc, char* argv[])
{
	/*{
	bConsoleOutput = false;
	CardDB DB;
	DB.LoadCardXML("cards.xml");

	ActiveDeck X(DB.CARD("Freddie"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));

	ActiveDeck Y(DB.CARD("Vyander"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));

	printf("X hash: %s\n",X.GetHash64().c_str());
	printf("Y hash: %s\n",Y.GetHash64().c_str());

	Branching B(X,Y);
	while (B.Evaluate());

	scanf("%s");
	}*/

#if !_DEBUG
	// executable uses shared memory to recieve parameters from API
	HANDLE hFileMapping = CreateFileMapping(
			INVALID_HANDLE_VALUE,    // use paging file
			NULL,                    // default security
			PAGE_READWRITE,          // read/write access
			0,                       // maximum object size (high-order DWORD)
			sizeof(EVAL_PARAMS),     // maximum object size (low-order DWORD)
			SM_NAME);                // name of mapping object
	if (hFileMapping == NULL)
	{
		printf("Could not create file mapping object (%d).\n",
			GetLastError());
		return 1;
	}
	EVAL_PARAMS* pEvalParams = (EVAL_PARAMS*)MapViewOfFile(hFileMapping,   // handle to map object
				FILE_MAP_ALL_ACCESS, // read/write permission
				0,
				0,
				sizeof(EVAL_PARAMS));
	if (pEvalParams == NULL)
	{
		printf("Could not map view of file (%d).\n",
			GetLastError());
		return 1;
	}

	//memcpy(buffer,(PVOID)pBuf,BUF_SIZE);
	//printf(buffer);

	bConsoleOutput = false;
	DB.LoadCardXML("cards.xml");
	DB.LoadRaidXML("raids.xml");

	ActiveDeck X,Y;
	DB.CreateDeck(pEvalParams->AtkDeck,X);
	DB.CreateDeck(pEvalParams->DefDeck,Y);

	memset(&pEvalParams->Result,0,sizeof(RESULTS));

	time_t t;

	time(&t);
	EvaluateInThreads(pEvalParams->Seed,X,Y,pEvalParams->RaidID,pEvalParams->Result,pEvalParams->GamesPerThread,pEvalParams->Threads,pEvalParams->Surge);
	time_t t1;
	time(&t1);
	pEvalParams->Seconds = (DWORD)t1-t;
	printf("Finished in %d sec\n",t1-t);

	UnmapViewOfFile(pEvalParams);
	CloseHandle(hFileMapping);

   return 1;
#else
	bConsoleOutput = false;
	CardDB DB;
	DB.LoadCardXML("cards.xml");
	DB.LoadMissionXML("missions.xml");
	DB.LoadRaidXML("raids.xml");

/*	ActiveDeck X(DB.CARD("Yurich"));
	X.Deck.push_back(DB.CARD("Colossus"));
	X.Deck.push_back(DB.CARD("Colossus"));
	X.Deck.push_back(DB.CARD("Colossus"));
	X.Deck.push_back(DB.CARD("Colossus"));
	X.Deck.push_back(DB.CARD("Colossus"));
	X.Deck.push_back(DB.CARD("Colossus"));
	X.Deck.push_back(DB.CARD("Colossus"));
	X.Deck.push_back(DB.CARD("Goliath"));
	X.Deck.push_back(DB.CARD("Demolition Bot"));
	X.Deck.push_back(DB.CARD("Dozer Tank"));
/*	ActiveDeck Y(DB.CARD("Yurich"));
	Y.Deck.push_back(DB.CARD("Colossus"));
	Y.Deck.push_back(DB.CARD("Colossus"));
	Y.Deck.push_back(DB.CARD("Colossus"));
	Y.Deck.push_back(DB.CARD("Colossus"));
	Y.Deck.push_back(DB.CARD("Colossus"));
	Y.Deck.push_back(DB.CARD("Colossus"));
	Y.Deck.push_back(DB.CARD("Colossus"));
	Y.Deck.push_back(DB.CARD("Goliath"));
	Y.Deck.push_back(DB.CARD("Demolition Bot"));
	Y.Deck.push_back(DB.CARD("Dozer Tank"));
	
/*	ActiveDeck X(DB.CARD("Dracorex"));
	X.Deck.push_back(DB.CARD("Azure Reaper"));
	X.Deck.push_back(DB.CARD("Azure Reaper"));
	X.Deck.push_back(DB.CARD("Azure Reaper"));
	X.Deck.push_back(DB.CARD("Azure Reaper"));
	X.Deck.push_back(DB.CARD("Sustainer Xolan"));
	X.Deck.push_back(DB.CARD("Sustainer Xolan"));
	X.Deck.push_back(DB.CARD("Sustainer Xolan"));
	X.Deck.push_back(DB.CARD("Vaporwing"));
	X.Deck.push_back(DB.CARD("Rifter"));
	X.Deck.push_back(DB.CARD("Apex"));

	ActiveDeck Y(DB.CARD("Dracorex"));
	Y.Deck.push_back(DB.CARD("Pummeller"));
	Y.Deck.push_back(DB.CARD("Pummeller"));
	Y.Deck.push_back(DB.CARD("Pummeller"));
	Y.Deck.push_back(DB.CARD("Pummeller"));
	Y.Deck.push_back(DB.CARD("Pummeller"));
	Y.Deck.push_back(DB.CARD("Blood Pool"));
	Y.Deck.push_back(DB.CARD("Blood Pool"));
	Y.Deck.push_back(DB.CARD("Blood Pool"));
	Y.Deck.push_back(DB.CARD("Draconian Queen"));
	Y.Deck.push_back(DB.CARD("Valefar"));

/*	ActiveDeck X(DB.CARD("Freddie"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	X.Deck.push_back(DB.CARD("Sharpshooter"));
	//X.Deck.push_back(DB.CARD("Rocket Infantry"));
	//ActiveDeck Y;
	//DB.GenRaidDeck(Y,11);
/*	ActiveDeck Y(DB.CARD("Vyander"));
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
*/	
/*	ActiveDeck Y(DB.CARD("Vyander"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));
	Y.Deck.push_back(DB.CARD("Kraken"));*/

	ActiveDeck X(DB.CARD("Vyander"));
	X.Deck.push_back(DB.CARD("Stealthy Niaq"));
	X.Deck.push_back(DB.CARD("Stealthy Niaq"));
	//X.Deck.push_back(DB.CARD("Heavy Infantry"));
	ActiveDeck Y(DB.CARD("Vyander"));
	//Y.Deck.push_back(DB.CARD("Chopper"));
	Y.Deck.push_back(DB.CARD("Predator"));
	Y.Deck.push_back(DB.CARD("Predator"));
	//Y.Deck.push_back(DB.CARD("Enclave Warlord"));
	//Y.Deck.push_back(DB.CARD("Enclave Warlord"));
	//Y.Deck.push_back(DB.CARD("Enclave Warlord"));


	printf("X hash: %s\n",X.GetHash64().c_str());
	printf("Y hash: %s\n",Y.GetHash64().c_str());

	Branching B(X,Y);
	while (B.Evaluate());
	//B.Evaluate();
	// X hash: P3AGBW+p
	// Y hash: QbDO+q

	ActiveDeck Z((const char*)"P3AGBW+p",DB.GetPointer());
	Z.PrintShort();
	ActiveDeck S((const char*)"QbDO+q",DB.GetPointer());
	S.PrintShort();

	RESULTS res;
	time_t t;

	time(&t);
	EvaluateInThreads(100500,X,Y,-1,res,2500,4);
	time_t t1;
	time(&t1);
	printf("Finished in %d sec\n",t1-t);
#endif
}

