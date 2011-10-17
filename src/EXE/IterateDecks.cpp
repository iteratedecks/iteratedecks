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
	bool OrderMatters;
};

int _tmain(int argc, char* argv[])
{
	/*{
	bConsoleOutput = false;

	DB.LoadCardXML("cards.xml");
	DB.LoadMissionXML("missions.xml");
	DB.LoadRaidXML("raids.xml");

	ActiveDeck mm119("RfBUDWD9E4FNFRFYFdFrFsGnG0frvOvT",DB.GetPointer()); // 
	// base deck:
	ActiveDeck basedeck("P+D2EDFrGn+n",DB.GetPointer()); // draco + 3 lance riders
	typedef vector <UINT>	VID;
	typedef set <UINT>		SID;
	VID Commanders;
	VID Cards;
	SID Exclude;
	//Exclude.insert(289); // DEBUG - EXCLUDE VALEFAR
	if (basedeck.Commander.IsDefined())
		Commanders.push_back(basedeck.Commander.GetId());
	for (UINT icmd=1000;icmd<2000;icmd++)
	{
		Card c = DB.GetCard(icmd);
		if (c.IsCard() && c.GetSet())
		{
			//printf("%d %d	%d	%s\n",c.GetRarity(),c.GetFaction(),icmd,c.GetName());
			if (c.GetRarity() >= RARITY_UNIQUE)// debug filter
				if ((!basedeck.Commander.IsDefined()) || (icmd != basedeck.Commander.GetId()))
					Commanders.push_back(icmd);
		}
	}
	for (UINT icard=0;icard<3000;icard++)
	{
		if ((icard >= 1000) && (icard < 2000)) // commanders
			continue;

		Card c = DB.GetCard(icard);
		if (c.IsCard() && c.GetSet())
		{
			//printf("%d %d	%d	%s\n",c.GetRarity(),c.GetFaction(),icard,c.GetName());
			//if ((rand() % 100) < 1)// debug filter
			if ((c.GetRarity() >= RARITY_RARE) && (c.GetFaction() == FACTION_IMPERIAL))
				if (Exclude.find(icard) == Exclude.end())
					Cards.push_back(icard);
		}
	}
	printf("%d : %d\n",Commanders.size(),Cards.size());
	UINT CardsSize = Cards.size();
#define PICK_DECK_SIZE	10
	UINT Picks[PICK_DECK_SIZE];

#define GAMES_COUNT	100
#define GAMES_EMUL	10
	float cchance = 0.0;
	ActiveDeck bestdeck;
	// start
	for (UINT ci=0;ci<Commanders.size();ci++)
	{
		printf("Commader: %d\n",Commanders[ci]);
		memset(Picks,0,sizeof(UINT) * PICK_DECK_SIZE);
		UINT cardcount = 1;
		for (;;Picks[0]++)
		{
			/* this works bad:
			0 0 0 0	invalid
			1 0 0 0
			2 0 0 0
			0 1 0 0	dupe
			1 1 0 0
			2 1 0 0
			0 2 0 0	dupe
			1 2 0 0	dupe
			2 2 0 0
			0 1 1 0	dupe
			1 1 1 0
			2 1 1 0
			*/
			/* this works good:
			1
			2
			3
			1 1
			1 2
			1 3
			2 2
			2 3
			3 3
			1 1 1
			1 1 2
			1 1 3
			1 2 2
			1 2 3
			1 3 3
			2 2 2
			2 2 3
			2 3 3
			3 3 3
			*/
/*			if (Picks[0] > CardsSize)
				for (UINT i=1;i<PICK_DECK_SIZE;i++)
					if (Picks[i] < CardsSize)
					{
						Picks[i]++;
						UINT ioffset = 0;
						// we can seed out some multiple Legendaries or Uniques here:
						if (((Picks[i]-1) < CardsSize) && (DB.GetCard(Cards[Picks[i]-1]).GetRarity() >= RARITY_UNIQUE))
							ioffset = 1; // skip em
						for (UINT k=0;k<i;k++)
							Picks[k] = Picks[i] + ioffset; // reset
						if (i > cardcount)
							cardcount = i;
						break;
					}
			if (Picks[0] > CardsSize)
				break; // end

			// check if picks are valid deck
			// we should have seeded out all multiple unique and multiple legendaries(clones) before
			// so now seed out decks that have multiple legendaries(deck can't have more than one)
			UINT iLegendaries = 0;
			for (UINT i=0;i<PICK_DECK_SIZE;i++)
				if ((Picks[i]) && (DB.GetCard(Cards[Picks[i]-1]).GetRarity() == RARITY_LEGENDARY))
				{
					iLegendaries++;
					if (iLegendaries > 1)
						break;					
				}
			if (iLegendaries > 1)
				continue; // skip this deck completely

			// current deck
			ActiveDeck cd(&DB.GetCard(Commanders[ci]));
			for (UINT i=0;i<PICK_DECK_SIZE;i++)
				if (Picks[i])
					cd.Add(&DB.GetCard(Cards[Picks[i]-1]));
				else
				{
					if (i < basedeck.Deck.size())
						cd.Add(basedeck.Deck[i].GetOriginalCard());
y,
			if (!basedeck.Deck.empty())
			{
				// need an extra check for unique and legendaries
				if (!cd.IsValid())
					continue;
			}
			//printf("%s\n",cd.GetHash64().c_str());
			//for (UINT i=0;i<PICK_DECK_SIZE;i++)
			//	printf("%d ",Picks[i]);
			//printf("\n");

			RESULTS res;
			for (UINT t=0;t<GAMES_EMUL;t++)
			{
				for (UINT k=0;k<GAMES_COUNT;k++)
				{
					ActiveDeck tm(mm119);
					ActiveDeck a(cd);

					Simulate(a,tm,res,false);
				}
				if (res.Win < (2 * GAMES_COUNT * cchance / 3))
					break; // chance is lower than 2/3 of current best, drop this
			}
			float newchance = (float)res.Win / (GAMES_COUNT * GAMES_EMUL);
			if (newchance >= cchance)
			{
				cchance = newchance;
				bestdeck = ActiveDeck(cd);
				printf("[%d] %.3f	%s\n",cardcount,cchance,bestdeck.GetHash64().c_str());
			}
		}
		//printf("%d variations overall\n",cnt);
	}
	}*/
/*	{
	bConsoleOutput = false;
	CardDB DB;
	DB.LoadCardXML("cards.xml");

	ActiveDeck m119("RZDWEjEmFKFNFPFdFfFgFsFtgNge",DB.GetPointer());
	ActiveDeck m115("RVAoBZBaBfB6EaFYFrGnfr",DB.GetPointer());
	ActiveDeck m99("RVAnCvDKDcDvFRFSGmgJvX",DB.GetPointer());
	ActiveDeck VR3("ReDWEjFJFKFNFPFlFsFtFuGxgfvW",DB.GetPointer());
	for (UINT i=0;i<1000;i++)
	{
		const Card *c = &DB.GetCard(i);
		if (c->IsCard() && (c->GetSet() > 0))
		{
#define GAMES_COUNT	100
#define GAMES_EMUL	10
			RESULTS res;
			for (UINT t=0;t<GAMES_EMUL;t++)
			{
				for (UINT k=0;k<GAMES_COUNT;k++)
				{
					ActiveDeck tm(VR3);
					ActiveDeck a(DB.CARD("Aiko"));		
					for (UCHAR l=0;l<GAMES_EMUL;l++)
						a.Add(c);

					Simulate(a,tm,res,false);
				}
				if (res.Win < (GAMES_COUNT / 2))
					break; // chance is lower then half, drop this
			}
			if (res.Win > 9 * GAMES_COUNT)
			{
				printf("%.3f	%s\n",(float)res.Win / (GAMES_COUNT * GAMES_EMUL),c->GetName());
			}
		}
	}

	scanf("%s");
	}
*/
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
	X.SetOrderMatters(pEvalParams->OrderMatters);

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
	DB.LoadCardXML("cards.xml");
	DB.LoadRaidXML("raids.xml");

	{
	//ActiveDeck X("PsDIfcfc",DB.GetPointer()),
	//	Y("Q4BhBvBwBxCDC1DIDJDNfvfwvI",DB.GetPointer());//
	ActiveDeck X,Y;
	DB.CreateDeck("Lord of Tartarus,Mawcor,Asylum,Asylum",X);
	DB.CreateDeck("Dracorex[1080],Carcass Scrounge,Blood Spout,Abomination,Hatchet,Beholder,Acid Spewer,Mawcor,Lummox,Pummeller,Blood Pool,Blood Wall,Impede Assault",Y);
	X.SetOrderMatters(true);

	RESULTS res;
	for (UINT k=0;k<1000;k++)
	{
		ActiveDeck x(X);
		ActiveDeck y(Y);		

		Simulate(x,y,res,false);
	}
	printf("%d\n",res.Win);
	}

	{
	RESULTS res;
	for (UINT i=0;10;i++)
	{
	ActiveDeck a("QCAXBJDd+nD+",DB.GetPointer());
	EvaluateRaidOnce(a,res,7);
	}
	}

	ActiveDeck m119("RZDWEjEmFKFNFPFdFfFgFsFtgNge",DB.GetPointer());
	for (UINT i=0;i<1000;i++)
	{
		const Card *c = &DB.GetCard(i);
		if (c->IsCard() && (c->GetSet() > 0))
		{
#define GAMES_COUNT	100
#define GAMES_EMUL	10
			RESULTS res;
			for (UINT t=0;t<10;t++)
			{
				for (UINT k=0;k<GAMES_COUNT;k++)
				{
					ActiveDeck tm(m119);
					ActiveDeck a(DB.CARD("Dracorex"));		
					for (UCHAR l=0;l<GAMES_EMUL;l++)
						a.Add(c);

					Simulate(a,tm,res,false);
				}
				if (res.Win < (GAMES_COUNT / 2))
					break; // chance is lower then half, drop this
			}
			if (res.Win > GAMES_COUNT * GAMES_EMUL / 2)
			{
				printf("%.3f	%s\n",(float)res.Win / GAMES_COUNT,c->GetName());
			}
		}
	}

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

