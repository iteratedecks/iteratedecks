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
// main module for EXE part, contains a lot of commented snippets that can be used as samples of classes usage
// also contains some threading evaluation routines

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

#define MAX_FILTER_ID_COUNT		50

CardDB DB; // just to make all easier ...
UINT MaxTurn = MAX_TURN;

void Simulate(ActiveDeck &tAtk, ActiveDeck &tDef, RESULTS &r, const UCHAR *CSIndex = 0, RESULT_BY_CARD *rbc = 0, bool bSurge = false)
{
/*
Should have given more credit to people that help improving...
Original source is by Hotshot2k4
http://www.kongregate.com/forums/65-tyrant/topics/201617-tyrant-war-scores-formula
but Fonzoland explained it so that I could understand:
All modes.
1. Winning gives a base score of 10 on fight/defense, 30 on surge. Surrender/stall count as a win for the defender.
2. There is a Уdamage bonusФ (all damage dealt to enemy commander, including overkill, in a certain range of turns), which is always capped at 10.
3. There is a Уspeed bonusФ for killing the enemy commander before a certain turn, which is always 5.
Auto mode.
All turns:
Winner damage bonus accumulates from the start.
Loser damage bonus accumulates from the start.
Speed bonus applies before turn 10.
Manual.
Let T be the time of the last manual decision, ie the turn when the manual player has exactly two cards in hand (usually 17/18 on fight/surge, for 10 card decks).
Turns 1 to T:
Winner damage bonus accumulates from card picking.
Loser damage bonus accumulates from card picking.
Speed bonus applies.
Turns T+1 to end:
Winner damage bonus accumulates from T.
Loser damage bonus accumulates from T.
Speed bonus applies before turn T+10.
In other words, it is the same as on auto, only the counters reset every time you choose a card. Defender never gets loser bonus during early stages, but attacker might if he damages the commander the turn before losing to the AI.
*/
	if (CSIndex && rbc)
		tAtk.SetFancyStatsBuffer(CSIndex,rbc);
	UCHAR iLastManualTurn = 0;
	UCHAR iAutoAtkDmg = 0, iAutoDefDmg = 0;
	for (UCHAR i=0; (i < MaxTurn);)
	{
		if (bSurge)
		{
			tDef.AttackDeck(tAtk);
			if (!tAtk.Commander.IsAlive())
			{
				r.LPoints+=10; // for winning 
				r.LAutoPoints+=10;
				if (tAtk.DamageToCommander >= 10)   // + points for dealing damage to enemy commander
					r.Points+=10;
				else
					r.Points+=tAtk.DamageToCommander;
				if (tDef.DamageToCommander >= 10)  // dying
					r.LPoints+=10;
				else
					r.LPoints+=tDef.DamageToCommander;
				iAutoAtkDmg += tAtk.DamageToCommander;
				iAutoDefDmg += tDef.DamageToCommander;
				if (iAutoAtkDmg > 10)
					r.AutoPoints += 10;
				else
					r.AutoPoints += iAutoAtkDmg;
				if (iAutoDefDmg > 10)
					r.LAutoPoints += 10;
				else
					r.LAutoPoints += iAutoDefDmg;
				r.Loss++;
				if (i < 10)
					r.LAutoPoints+=5; // +5 points for losing by turn 10 on auto
				if ((!iLastManualTurn) || (i < iLastManualTurn + 10))
					r.LPoints+=5; // +5 points for losing by turn T+10 
				break;
			}
			i++;
		}
		if (CSIndex && rbc)
		{
			if (tAtk.Deck.size() == 1)
			{
				// last card is about to be played
				UINT id = tAtk.Deck[0].GetId(); // it's ID
				// play variation without this card
				rbc[CSIndex[id]].WLGames++;
				ActiveDeck xwl(tAtk),ywl(tDef);
				xwl.Deck.clear();
				xwl.SetFancyStatsBuffer(0,0); // don't inherit buffers here
				ywl.SetFancyStatsBuffer(0,0); // don't inherit buffers here
				for (UCHAR iwl=i; (iwl < MaxTurn); )
				{
					if (bSurge && (iwl > i)) // double defence turn without this check
					{	
						ywl.AttackDeck(xwl);
						iwl++;
						if (!xwl.Commander.IsAlive())
						{
							rbc[CSIndex[id]].WLLoss++;
							break;
						}
					}
					xwl.AttackDeck(ywl);
					iwl++;
					if (!ywl.Commander.IsAlive())
					{
						rbc[CSIndex[id]].WLWin++;
						break;
					}
					if (!bSurge)
					{
						ywl.AttackDeck(xwl);
						iwl++;
						if (!xwl.Commander.IsAlive())
						{
							rbc[CSIndex[id]].WLLoss++;
							break;
						}
					}					
				}
			}
		}
		if (tAtk.Deck.size() >= 2)
		{
			iAutoAtkDmg += tAtk.DamageToCommander;
			iAutoDefDmg += tDef.DamageToCommander;
			tAtk.DamageToCommander = 0; // reset
			tDef.DamageToCommander = 0; // reset
		}
		if (tAtk.Deck.size() == 2)
			iLastManualTurn = i;
		tAtk.AttackDeck(tDef);
		if (!tDef.Commander.IsAlive())
		{
			r.Win++;
			r.Points+=10; // +10 points for winning 
			r.AutoPoints+=10;
			if (tAtk.DamageToCommander >= 10)   // + points for dealing damage to enemy commander
				r.Points+=10;
			else
				r.Points+=tAtk.DamageToCommander;
			if (tDef.DamageToCommander >= 10)  // suffering damage
				r.LPoints+=10;
			else
				r.LPoints+=tDef.DamageToCommander;
			iAutoAtkDmg += tAtk.DamageToCommander;
			iAutoDefDmg += tDef.DamageToCommander;
			if (iAutoAtkDmg > 10)
				r.AutoPoints += 10;
			else
				r.AutoPoints += iAutoAtkDmg;
			if (iAutoDefDmg > 10)
				r.LAutoPoints += 10;
			else
				r.LAutoPoints += iAutoDefDmg;
			if (bSurge)
			{
				r.Points+=20; // +20 points for winning on surge
				r.AutoPoints+=20;
			}
			if (i < 10)
				r.AutoPoints+=5; // +5 points for winning by turn 10 on auto
			if ((!iLastManualTurn) || (i < iLastManualTurn + 10))
				r.Points+=5; // +5 points for winning by turn T+10
			break;
		}
		i++;
		if (!bSurge)
		{
			tDef.AttackDeck(tAtk);
			if (!tAtk.Commander.IsAlive())
			{
				r.LPoints+=10; // for winning 
				r.LAutoPoints+=10;
				if (tAtk.DamageToCommander >= 10)   // + points for dealing damage to enemy commander
					r.Points+=10;
				else
					r.Points+=tAtk.DamageToCommander;
				if (tDef.DamageToCommander >= 10)  // dying
					r.LPoints+=10;
				else
					r.LPoints+=tDef.DamageToCommander;
				iAutoAtkDmg += tAtk.DamageToCommander;
				iAutoDefDmg += tDef.DamageToCommander;
				if (iAutoAtkDmg > 10)
					r.AutoPoints += 10;
				else
					r.AutoPoints += iAutoAtkDmg;
				if (iAutoDefDmg > 10)
					r.LAutoPoints += 10;
				else
					r.LAutoPoints += iAutoDefDmg;
				r.Loss++;
				if (i < 10)
					r.LAutoPoints+=5; // +5 points for losing by turn 10 on auto
				if ((!iLastManualTurn) || (i < iLastManualTurn + 10))
					r.LPoints+=5; // +5 points for losing by turn T+10
				break;
			}
			i++;
		}
	}
	// check if cards stayed in deck - we finished before they could be played, lol
	if (CSIndex && rbc && (!tAtk.Deck.empty()))
	{
		// well, we can't actually pick all the remaining cards and assign this win/loss to all of them
		// we have to pick ONE LAST
		// random? but order could matter
		// remove all the cards except for last to be picked
		while (tAtk.Deck.size() > 1)
			tAtk.PickNextCard(true);
		UINT id = tAtk.Deck[0].GetId(); // thats the last card
		// this card did not participate in current game
		rbc[CSIndex[id]].WLGames++;
		if (!tAtk.Commander.IsAlive())
			rbc[CSIndex[id]].WLLoss++;
		if (!tDef.Commander.IsAlive())
			rbc[CSIndex[id]].WLWin++;
	}
	// stalled
	if (tAtk.Commander.IsDefined() && tDef.Commander.IsDefined() && tAtk.Commander.IsAlive() && tDef.Commander.IsAlive())
	{
		// stall = loss
		r.LPoints+=10;
		r.LAutoPoints+=10;
		if (tAtk.DamageToCommander >= 10)   // + points for dealing damage to enemy commander
			r.Points+=10;
		else
			r.Points+=tAtk.DamageToCommander;
		if (tDef.DamageToCommander >= 10)  // suffered damage
			r.LPoints+=10;
		else
			r.LPoints+=tDef.DamageToCommander;
		iAutoAtkDmg += tAtk.DamageToCommander;
		iAutoDefDmg += tDef.DamageToCommander;
		if (iAutoAtkDmg > 10)
			r.AutoPoints += 10;
		else
			r.AutoPoints += iAutoAtkDmg;
		if (iAutoDefDmg > 10)
			r.LAutoPoints += 10;
		else
			r.LAutoPoints += iAutoDefDmg;
	}
	tAtk.SweepFancyStatsRemaining();
	r.Games++;
}

struct EVAL_THREAD_PARAMS
{
	const ActiveDeck *Atk;
	const ActiveDeck *Def;
	int RaidID;
	DWORD gamesperthread;
	DWORD Seed;
	RESULTS r;
	const UCHAR *CSIndex;
	RESULT_BY_CARD rbc[DEFAULT_DECK_RESERVE_SIZE+1];
	bool bSurge;
	int *pState;
	UCHAR TournamentMode;
};

void EvaluateRaidOnce(const ActiveDeck gAtk, RESULTS &r, const UCHAR *CSIndex/* = 0*/, RESULT_BY_CARD *rbc/* = 0*/, DWORD RaidID)
{
	ActiveDeck tAtk(gAtk);
	ActiveDeck tDef;
	DB.GenRaidDeck(tDef,RaidID);

	if (CSIndex && rbc)
		tAtk.SetFancyStatsBuffer(CSIndex,rbc);
	UCHAR iLastManualTurn = 0;
	UCHAR iAutoAtkDmg = 0;
	for (UCHAR i=0; (i < MaxTurn); )
	{
		if (CSIndex && rbc)
		{
			if (tAtk.Deck.size() == 1)
			{
				// last card is about to be played
				UINT id = tAtk.Deck[0].GetId(); // it's ID
				// play variation without this card
				rbc[CSIndex[id]].WLGames++;
				ActiveDeck xwl(tAtk),ywl(tDef);
				xwl.Deck.clear();
				for (UCHAR iwl=i; (iwl < MaxTurn); )
				{
					xwl.AttackDeck(ywl);
					iwl++;
					if (!ywl.Commander.IsAlive())
					{
						rbc[CSIndex[id]].WLWin++;
						break;
					}
					ywl.AttackDeck(xwl);
					iwl++;
					if (!xwl.Commander.IsAlive())
					{
						rbc[CSIndex[id]].WLLoss++;
						break;
					}
				}
			}
		}
		if (tAtk.Deck.size() >= 2)
		{
			iAutoAtkDmg += tAtk.DamageToCommander;
			tAtk.DamageToCommander = 0; // reset
		}
		if (tAtk.Deck.size() == 2)
			iLastManualTurn = i;
		tAtk.AttackDeck(tDef);
		if (!tDef.Commander.IsAlive())
		{
			r.Win++;
			r.Points+=10; // +10 points for winning 
			r.AutoPoints+=10;
			if (tAtk.DamageToCommander >= 10)   // + points for dealing damage to enemy commander
				r.Points+=10;
			else
				r.Points+=tAtk.DamageToCommander;
			iAutoAtkDmg += tAtk.DamageToCommander;
			if (iAutoAtkDmg >= 10)   // + points for dealing damage to enemy commander
				r.AutoPoints+=10;
			else
				r.AutoPoints+=iAutoAtkDmg;
			if (i < 10)
				r.AutoPoints+=5; // +5 points for winning by turn 10 on auto
			if ((!iLastManualTurn) || (i < iLastManualTurn + 10))
				r.Points+=5; // +5 points for winning by turn T+10
			break;
		}
		i++;
		tDef.AttackDeck(tAtk);
		if (!tAtk.Commander.IsAlive())
		{
			r.Loss++;
			break;
		}
		i++;
	}
	// check if cards stayed in deck - we finished before they could be played, lol
	if (CSIndex && rbc && (!tAtk.Deck.empty()))
	{
		// well, we can't actually pick all the remaining cards and assign this win/loss to all of them
		// we have to pick ONE LAST
		// random? but order could matter
		// remove all the cards except for last to be picked
		while (tAtk.Deck.size() > 1)
			tAtk.PickNextCard(true);
		UINT id = tAtk.Deck[0].GetId(); // thats the last card
		// this card did not participate in current game
		rbc[CSIndex[id]].WLGames++;
		if (!tAtk.Commander.IsAlive())
			rbc[CSIndex[id]].WLLoss++;
		if (!tDef.Commander.IsAlive())
			rbc[CSIndex[id]].WLWin++;
	}
	tAtk.SweepFancyStatsRemaining();
	r.Games++;
}

static unsigned int __stdcall ThreadFunc(void *pvParam)
{
	EVAL_THREAD_PARAMS *p = (EVAL_THREAD_PARAMS *)pvParam;
	srand((unsigned)p->Seed); // it seems like each thread shares seed with others before it starts, so we should reset seed or we will gate the same random sequences in all threads
	RESULTS lr;
	RESULT_BY_CARD rbc[DEFAULT_DECK_RESERVE_SIZE+1];
	UINT customcount = DB.GetCustomCount();
	UINT customindex = 100; // doesn't matter if it is not 0
	ActiveDeck customdeck;
	if (p->Atk->IsValid(true))
		for (DWORD i=0;(i<p->gamesperthread) && (*(p->pState) >= 0);i++)
		{
			if (p->RaidID < 0)
			{
				ActiveDeck Atk(*p->Atk);
				ActiveDeck Def(*p->Def);
				if (!Def.IsValid(true))
				{
					UINT idx = i * customcount / p->gamesperthread;
					if (idx != customindex)
					{
						DB.GetCustomDeck(idx,customdeck);
						customindex = idx;
					}
					Def = customdeck;
				}
				if (p->TournamentMode > 0)
				{
					if (p->bSurge)
						Def.DelayFirstCard();
					else
						Atk.DelayFirstCard();
				}
				Simulate(Atk,Def,lr,p->CSIndex,rbc,p->bSurge);
			}
			else
				EvaluateRaidOnce(*(p->Atk),lr,p->CSIndex,rbc,(DWORD)p->RaidID);
		}
	//_endthread();
	p->r.Add(lr);
	for (UCHAR m=0;m<DEFAULT_DECK_SIZE+1;m++)
		p->rbc[m].Add(rbc[m]);
	return (UINT)p;
}

void EvaluateInThreads(DWORD Seed, const ActiveDeck &gAtk, const ActiveDeck &gDef, int RaidID, RESULTS &ret, RESULT_BY_CARD *rbc, int &State, DWORD gamesperthread, DWORD threadscount = 1, bool bSurge = false, UCHAR TournamentMode = 0)
{
	// create Index
	UCHAR CSIndex[CARD_MAX_ID];
	CSIndex[gAtk.Commander.GetId()] = 0;
	rbc[0].Id = gAtk.Commander.GetId();
	for (UCHAR i=0;i<gAtk.Deck.size();i++)
	{
		UCHAR idx = 0;
		for (idx=0;idx<DEFAULT_DECK_RESERVE_SIZE+1;idx++)
			if (rbc[idx].Id == gAtk.Deck[i].GetId())
				break;
			else
				if (!rbc[idx].IsValid())
				{
					rbc[idx].Id = i;
					break;
				}
		_ASSERT(idx);
		CSIndex[gAtk.Deck[i].GetId()] = idx;
		rbc[idx].Id = gAtk.Deck[i].GetId();
	}
	//
	if (threadscount <= 1)
	{
		srand((unsigned)Seed);
		UINT customcount = DB.GetCustomCount();
		UINT customindex = 100; // doesn't matter if it is not 0
		ActiveDeck customdeck;
		for (DWORD i=0;(i<gamesperthread) && (State >= 0);i++)
		{
			if (RaidID < 0)
			{
				ActiveDeck tAtk(gAtk);
				ActiveDeck tDef(gDef);
				
				if (TournamentMode > 0)
				{
					if (bSurge)
						tDef.DelayFirstCard();
					else
						tAtk.DelayFirstCard();
				}
				if (!tDef.IsValid())
				{
					const UINT idx = i * customcount / gamesperthread;
					if (idx != customindex)
					{
						DB.GetCustomDeck(idx,customdeck);
						customindex = idx;
					}
					tDef = customdeck;
				}
				Simulate(tAtk,tDef,ret,CSIndex,rbc,bSurge);
			}
			else
				EvaluateRaidOnce(gAtk,ret,CSIndex,rbc,(DWORD)RaidID);
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
			parms[i].CSIndex = CSIndex;
			parms[i].RaidID = RaidID;
			parms[i].gamesperthread = gamesperthread;
			parms[i].Seed = Seed + i; // offset seed or we will have same results for all threads
			parms[i].bSurge = bSurge;
			parms[i].pState = &State;
			parms[i].TournamentMode = TournamentMode;

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
			for (UCHAR m=0;m<DEFAULT_DECK_SIZE+1;m++)
				rbc[m].Add(parms[i].rbc[m]);
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
	RESULT_BY_CARD ResultByCard[DEFAULT_DECK_RESERVE_SIZE+1];
	DWORD Seconds;
	int RaidID;
	bool Surge;
	bool OrderMatters;
	// wildcard
	int WildcardId;
	int WildFilterType;
	int WildFilterRarity;
	int WildFilterFaction;
	int WildFilterInclude[MAX_FILTER_ID_COUNT];
	int WildFilterExclude[MAX_FILTER_ID_COUNT];
	UINT MaxTurn;
	// control
	int State; // set to -1 to stop
	// tournament mode
	UCHAR TournamentMode; // 0 - no tourney
};

int _tmain(int argc, char* argv[])
{
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

	MaxTurn = pEvalParams->MaxTurn;

	ActiveDeck X,Y;
	DB.CreateDeck(pEvalParams->AtkDeck,X);
	if (DB.LoadDecks(pEvalParams->DefDeck) == -2) // FNF
		DB.CreateDeck(pEvalParams->DefDeck,Y);
	X.SetOrderMatters(pEvalParams->OrderMatters);

	memset(&pEvalParams->Result,0,sizeof(RESULTS));
	memset(&pEvalParams->ResultByCard,0,sizeof(RESULT_BY_CARD) * (DEFAULT_DECK_RESERVE_SIZE + 1));	

	time_t t;

	time(&t);
	if (pEvalParams->WildcardId)
	{
		typedef set <UINT>		SID;
		SID CardPool;
		for (UINT k=0;k<MAX_FILTER_ID_COUNT;k++)
		{
			if (pEvalParams->WildFilterInclude[k])
				CardPool.insert(pEvalParams->WildFilterInclude[k]);
			else
				break;
		}
		if (pEvalParams->WildcardId < 0)
		{
			// remove all non-commander cards from pool
			for (SID::iterator si = CardPool.begin(); si != CardPool.end(); si++)
				while ((si != CardPool.end()) && (DB.GetCard(*si).GetType() != TYPE_COMMANDER))
					si = CardPool.erase(si);
			// commander
			for (UINT icmd=1000;icmd<2000;icmd++)
			{
				Card c = DB.GetCard(icmd);
				if (c.IsCard() && c.GetSet())
				{
					UCHAR ifilter = 0;
					for (UINT k=pEvalParams->WildFilterRarity;k;k/=10)
						if (c.GetRarity()+1 == k%10) // need an offset here since Common = 0
						{
							ifilter++;
							break;
						}
					for (UINT k=pEvalParams->WildFilterFaction;k;k/=10)
						if (c.GetFaction() == k%10)
						{
							ifilter++;
							break;
						}
					if (ifilter >= 2)
						CardPool.insert(icmd);
				}
			}
		}
		else
		{
			// remove all commander cards from pool
			for (SID::iterator si = CardPool.begin(); si != CardPool.end(); si++)
				while ((si != CardPool.end()) && (DB.GetCard(*si).GetType() == TYPE_COMMANDER))
					si = CardPool.erase(si);
			// card in deck
			for (VCARDS::iterator vi=X.Deck.begin();vi!=X.Deck.end();vi++)
				if (vi->GetId() == pEvalParams->WildcardId)
				{
					vi = X.Deck.erase(vi); // remove that card
					break; // just replace one
				}
			for (UINT icard=0;icard<3000;icard++)
			{
				if ((icard >= 1000) && (icard < 2000)) // commanders
					continue;

				Card c = DB.GetCard(icard);
				if (c.IsCard() && c.GetSet())
				{
					UCHAR ifilter = 0;
					for (UINT k=pEvalParams->WildFilterType;k;k/=10)
						if (c.GetType() == k%10)
						{
							ifilter++;
							break;
						}
					for (UINT k=pEvalParams->WildFilterRarity;k;k/=10)
						if (c.GetRarity() + 1 == k%10) // need an offset here since Common = 0
						{
							ifilter++;
							break;
						}
					for (UINT k=pEvalParams->WildFilterFaction;k;k/=10)
						if (c.GetFaction() == k%10)
						{
							ifilter++;
							break;
						}
					if (ifilter >= 3)
						CardPool.insert(icard);
				}
			}
		}
		// apply exclude filter
		for (UINT k=0;k<MAX_FILTER_ID_COUNT;k++)
		{
			if (pEvalParams->WildFilterExclude[k])
			{
				SID::iterator si = CardPool.find(pEvalParams->WildFilterExclude[k]);
				if (si != CardPool.end())
					CardPool.erase(si);
			}
			else
				break;
		}
		if (CardPool.empty())
			pEvalParams->WildcardId = -1; // tell API that there were no cards in filter

		UINT BestCard = 0;
		for (SID::iterator si=CardPool.begin();si!=CardPool.end();si++)
		{
			ActiveDeck x(X);
			if (pEvalParams->WildcardId < 0)
				x.Commander = PlayedCard(&DB.GetCard(*si));
			else
				x.Deck.push_back(&DB.GetCard(*si));
			if (!x.IsValid())
				continue;
			RESULTS lresult;
			RESULT_BY_CARD lrbc[DEFAULT_DECK_RESERVE_SIZE+1];
			EvaluateInThreads(pEvalParams->Seed,x,Y,pEvalParams->RaidID,lresult,lrbc,pEvalParams->State,pEvalParams->GamesPerThread,pEvalParams->Threads,pEvalParams->Surge,pEvalParams->TournamentMode);
			if (lresult.Win > pEvalParams->Result.Win)
			{
				pEvalParams->Result = lresult;
				memcpy(pEvalParams->ResultByCard,lrbc,sizeof(lrbc));
				BestCard = *si;
			}
		}
		if (BestCard)
			pEvalParams->WildcardId = BestCard;
	}
	else // simple eval
		EvaluateInThreads(pEvalParams->Seed,X,Y,pEvalParams->RaidID,pEvalParams->Result,pEvalParams->ResultByCard,pEvalParams->State,pEvalParams->GamesPerThread,pEvalParams->Threads,pEvalParams->Surge,pEvalParams->TournamentMode);
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
	DB.LoadMissionXML("missions.xml");
	DB.SaveMissionDecks("c:\\pun.txt");

	ActiveDeck z("PoAv",DB.GetPointer());
	ActiveDeck x("PoFZ",DB.GetPointer());

	DB.CreateDeck("Emanuel[1126],Tiamat,Trident,Helios,Poseidon,Titan,Executioner,Wasteland Skimmer,Hellion,Elite Diver,Revolution,Electromagnetic Pulse,Mortar Bunker,Petrol Rig",z);
	DB.CreateDeck("Duncan,Salvager,Cannon Walker,Micromech,Juggernaut,Dozer Tank,Command Center",x);


	RESULTS r;
	for (UINT k=0;k<1000;k++)
	{
		ActiveDeck a(x),b(z);
		Simulate(a,b,r);
	}
	printf("%d\n",r.Win);
	return 0;
	// parameter weights:
	// attack
	// health
	// wait
	// skills cost/weights(should take from xml for now)
	// skill weight for ALL 10 targets
	// skill weight for FACTION condition
	// - set cost (set + rarity, aggregated)

	// create matrix
	/*
#define PARAMS_COUNT	4
	UINT rcount = 0;
	double mp[PARAMS_COUNT][1000],F[1000];
	for (UINT i=0000;i<1000;i++)
	{
		const Card *c = &DB.GetCard(i);
		float fo = 0, fd = 0;
		if (c->IsCard()/* && (c->GetWait() == 3)&& (c->GetWait() == 1) && (c->GetRarity() >= RARITY_COMMON) && (c->GetSet() != 0))
		{
			//printf("%s %d %d [%d] ",c->GetName(),c->GetAttack(),c->GetHealth(),c->GetWait());
			printf("%d=	%d	%d	[%d]	",c->GetRarity(),c->GetAttack(),c->GetHealth(),c->GetWait());
			//bool bSkip = false;
			//float ss = 0.0;
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
				if (DB.Skills[aid].IsPassive)
					fd += c->GetAbility(aid) * DB.Skills[aid].CardValue;
				else
					fo += c->GetAbility(aid) * DB.Skills[aid].CardValue * fmod;
				//f += c->GetAbility(aid) * DB.Skills[aid].CardValue;/* * fmod /** (1 + DB.Skills[aid].IsPassive) / 1/;
				//printf("%d : %d x %.1f = %.1f | ",aid,c->GetAbility(aid),DB.Skills[aid].CardValue,c->GetAbility(aid) * DB.Skills[aid].CardValue);
			}
			//printf("%.1f + %.1f	%.1f	|",fd,fo,fo+fd+2.5*c->GetAttack()+1.5*(c->GetHealth()/*-1/)-2.5*c->GetWait());
			//printf("	%.1f\n",fo / (c->GetWait() + 1)+fd+2.5*c->GetAttack() / (c->GetWait() + 0.5)+1.5*(c->GetHealth()/-1/));
			printf("	D %.1f	O- %.1f	O* %.1f\n",
				fd + 1.5*(c->GetHealth()),
				fo + 2.5*c->GetAttack() - 2.5*c->GetWait(),
				fo / (c->GetWait() + 0.5) + 2.5*c->GetAttack() / (c->GetWait() + 0.5)
				);
			mp[0][rcount] = c->GetAttack();
			mp[1][rcount] = c->GetHealth();
			mp[2][rcount] = fd;
			mp[3][rcount] = fo;
			//mp[4][rcount] = c->GetWait() + 1; // may want to try and filter out with constant wait first
			F[rcount] = c->GetRarity()+1;
			rcount++;
			// attack = 6
			// health = 3
			//printf("%.1f	%.1f	[%d]	%s\n",f,f+(c->GetHealth()-1) * 2 + c->GetAttack() * 3,c->GetRarity(),c->GetName());
			//printf("\n");
		}
	}
	printf("MP:\n");
	for (UINT i=0;i<rcount;i++)
	{
		for (UINT k=0;k<PARAMS_COUNT;k++)
		{
			printf("%.2f ",mp[k][i]);
		}
		printf("= %.2f ",F[i]);
		printf("\n");
	}
	// normalize matrix
	/*
	1 2 3
	4 5 6
	  x
	1 4
	2 5
	3 6	
	/
	double A[PARAMS_COUNT][PARAMS_COUNT],f[PARAMS_COUNT];
	for (UINT i=0;i<PARAMS_COUNT;i++)
	{
		for (UINT k=0;k<PARAMS_COUNT;k++)
		{
			double zf = 0.0;
			for (UINT z=0;z<rcount;z++)
			{
				zf += mp[i][z] * mp[k][z];
			}
			A[i][k] = zf;
		}
		double zf = 0.0;
		for (UINT z=0;z<rcount;z++)
			zf += F[z] * mp[i][z];
		f[i] = zf;
	}

	printf("NM:\n");
	for (UINT i=0;i<PARAMS_COUNT;i++)
	{
		for (UINT k=0;k<PARAMS_COUNT;k++)
		{
			printf("%.2f ",A[i][k]);
		}
		printf("= %.2f ",f[i]);
		printf("\n");
	}

	// this snippet is from my 2003 year classes...
	// simple iteration method
	{
		double B[PARAMS_COUNT][PARAMS_COUNT],b[PARAMS_COUNT],r[PARAMS_COUNT],x[PARAMS_COUNT],xprev[PARAMS_COUNT],temp[PARAMS_COUNT],e=0.1101;
		int i,n,k,j;
		bool flag;

		{
			//приводим систему Ax=f к виду x=Bx+b
			for (int i=0; i<PARAMS_COUNT; ++i)
			{
				b[i]=f[i]/A[i][i];
				for (int j=0; j<PARAMS_COUNT; ++j)
				{
					if (i!=j)
						B[i][j]=-A[i][j]/A[i][i];
					else
						B[i][j]=0;
				};
			};

			//метод простых итераций
			memcpy(x,b,sizeof(x));
			n=0;
			do
			{
				++n;
				memcpy(temp,x,sizeof(x));
				memcpy(x,xprev,sizeof(x));
				memcpy(xprev,temp,sizeof(x));
				for (j=0; j<PARAMS_COUNT; ++j)
				{
					x[j]=b[j];
					for (k=0; k<PARAMS_COUNT; ++k)
						x[j]+=B[j][k]*xprev[k];
				}

				//вычисление вектора нев€зки приближени€ x
				for (i=0; i<PARAMS_COUNT; ++i)
				{
					r[i]=x[i];
					for (j=0; j<PARAMS_COUNT; ++j)
					r[i]-=B[i][j]*x[j];
					r[i]-=b[i];
				}

				int i1=0;
				for (i=0; i<PARAMS_COUNT; ++i)
					if (fabs(r[i1])<e)
						++i1;

					if (i1<PARAMS_COUNT) flag=true;
					else flag=false;
			} while (flag);

			//вывод решени€
			for (k=0; k<PARAMS_COUNT; k++)
				printf("x%d = %.2f\n",k,x[k]);
				//cout<<"x"<<k<<" = "<<x[k]<<endl;

			//вывод вектора нев€зки
			printf("dispersion:\n");
			//cout<<"vector nevyazki:"<<endl;
			for (i=0; i<PARAMS_COUNT; i++)
			{
				e=0;
				for (j=0; j<PARAMS_COUNT; j++)
					e+=A[i][j]*x[j];
				printf("   %.2f\n",e-f[i]);
				//cout<<"   "<<e-f[i]<<endl;
			}
			//вывод количества потребовавшихс€ итераций
			printf("Number of iterations: %d\n",n);
			//cout<<"Number of iterations: "<<n<<endl;
			//cout<<endl;
		}
	}*/

//////////////////********************
	for (UINT i=0000;i<1000;i++)
	{
		const Card *c = &DB.GetCard(i);
		float f = 0;
		if (c->IsCard() && (c->GetWait() == 1) && (c->GetRarity() >= RARITY_COMMON) && (c->GetSet() != 0))
		{
			//printf("%s ",c->GetName());
			for (UCHAR k=0;k<c->GetAbilitiesCount();k++)
			{
				UCHAR aid = c->GetAbilityInOrder(k);
				UCHAR cnt = c->GetTargetCount(aid);
				if (cnt < 1)
					cnt = 1;
				float fmod = (cnt + 9) / 10;//(cnt + 1) / 2;
				if (c->GetTargetFaction(aid) != FACTION_NONE)
					fmod /= 2;
				f += c->GetAbility(aid) * DB.Skills[aid].CardValue;/* * fmod /** (1 + DB.Skills[aid].IsPassive) / 1*/;
				//printf("%d : %d x %.1f = %.1f | ",aid,c->GetAbility(aid),DB.Skills[aid].CardValue,c->GetAbility(aid) * DB.Skills[aid].CardValue);
			}
			// attack = 6
			// health = 3
			printf("%.1f	%.1f	[%d]	%s\n",f,f+(c->GetHealth()-1) * 2 + c->GetAttack() * 3,c->GetRarity(),c->GetName());
			//printf("\n");
		}
	}

	
	/*bConsoleOutput = true;
	DB.LoadRaidXML("raids.xml");
	{
		ActiveDeck X("QHB++pB+",DB.GetPointer()),Y;
		Y.Commander = DB.CARD("Freddie");
		RESULTS res;
		RESULT_BY_CARD rescs[DEFAULT_DECK_SIZE+1];

		EvaluateInThreads(0,X,Y,-1,res,rescs,2000,1);

		printf("%d\n",res.Win);
		for (UINT i=0;i<DEFAULT_DECK_SIZE+1;i++)
			rescs[i].Print();
		scanf("%c");
	}*/
	{
		//ActiveDeck X("PsDIfcfc",DB.GetPointer()),
		//	Y("Q4BhBvBwBxCDC1DIDJDNfvfwvI",DB.GetPointer());//
		
		//ActiveDeck X("P9Dw",DB.GetPointer());
		//ActiveDeck Y(/*"ReFF"*/"ReAo",DB.GetPointer());
		ActiveDeck X("QVDw+kD5EFE2+jH8",DB.GetPointer()),Y("RkBvBxB0DNDhDnE0FAFZGrH8IBfZfvvQ",DB.GetPointer());
		//ActiveDeck X("QVA2A2CcE+E+Fg+jGpGr"
			//"QVA2CcCtE+E+Fg+jGpGr" // best BR4 deck
			//"QVB0DN+lEhfvfvfv" // pumpool
		//	,DB.GetPointer()),Y("RdAXCCDIDNDdEGEREoFBFHFbGWfogBu4",DB.GetPointer());
		
		
		//ActiveDeck X,Y;
		//DB.CreateDeck("Lord of Tartarus,Mawcor,Asylum,Asylum,Chaos Wave",X);
		//DB.CreateDeck("Dracorex[1080],Carcass Scrounge,Blood Spout,Abomination,Hatchet,Beholder,Acid Spewer,Mawcor,Lummox,Pummeller,Blood Pool,Blood Wall,Impede Assault",Y);
		//X.SetOrderMatters(true);

		RESULTS res;
		RESULT_BY_CARD rescs[DEFAULT_DECK_SIZE+1];
		// Index
		UCHAR CSIndex[CARD_MAX_ID];
		CSIndex[X.Commander.GetId()] = 0;
		rescs[0].Id = X.Commander.GetId();
		for (UCHAR i=0;i<X.Deck.size();i++)
		{
			UCHAR idx = 0;
			for (idx=0;idx<DEFAULT_DECK_SIZE+1;idx++)
				if (rescs[idx].Id == X.Deck[i].GetId())
					break;
				else
					if (!rescs[idx].IsValid())
					{
						rescs[idx].Id = i;
						break;
					}
			_ASSERT(idx);
			CSIndex[X.Deck[i].GetId()] = idx;
			rescs[idx].Id = X.Deck[i].GetId();
		}
		//
		UINT games = 1000;
		for (UINT k=0;k<games;k++)
		{
			ActiveDeck x(X);
			ActiveDeck y(Y);

			x.SetFancyStatsBuffer(CSIndex,rescs);

			//Simulate(x,y,res,false);

			for (UCHAR i=0; (i < MaxTurn); i++)
			{
				if (x.Deck.size() == 1)
				{
					// last card is about to be played
					UINT id = x.Deck[0].GetId(); // it's ID
					// play variation without this card
					rescs[CSIndex[id]].WLGames++;
					ActiveDeck xwl(x),ywl(y);
					for (UCHAR iwl=i; (iwl < MaxTurn); iwl++)
					{
						xwl.AttackDeck(ywl);
						if (!ywl.Commander.IsAlive())
						{
							rescs[CSIndex[id]].WLWin++;
							break;
						}
						ywl.AttackDeck(xwl);
						if (!xwl.Commander.IsAlive())
						{
							rescs[CSIndex[id]].WLLoss++;
							break;
						}
					}
				}
				x.AttackDeck(y);
				if (!y.Commander.IsAlive())
				{
					res.Win++;
					// sweep fs
					x.SweepFancyStatsRemaining(); // this is wrong - i should sweep remaining even on losses in case of fear based enemy deck or any cards still present
					break;
				}
				y.AttackDeck(x);
				if (!x.Commander.IsAlive())
				{
					res.Loss++;
					break;
				}
			}
		}
		printf("%d\n",res.Win);
		for (UINT i=0;i<DEFAULT_DECK_SIZE+1;i++)
			rescs[i].Print();
	}

	{
	RESULTS res;
	for (UINT i=0;10;i++)
	{
	ActiveDeck a("QCAXBJDd+nD+",DB.GetPointer());
	EvaluateRaidOnce(a,res,0,0,7);
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

					Simulate(a,tm,res);
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
	int State;
	time_t t;

	time(&t);
	EvaluateInThreads(100500,X,Y,-1,res,0,State,2500,4);
	time_t t1;
	time(&t1);
	printf("Finished in %d sec\n",t1-t);
#endif
	//printf("%d\n",sizeof(EVAL_PARAMS));
	/*DB.LoadCardXML("cards.xml");
	ActiveDeck D("QVB0DN+lEhfvfvfv",DB.GetPointer());
	printf("%d\n",D.IsValid());
	scanf("%c");*/
	/*DB.LoadRaidXML("raids.xml");
	DB.LoadDecks("C:\\Users\\NT\\Documents\\Visual Studio 2008\\Projects\\EvaluateDecks\\bin\\decks\\raid\\ladolcevita.txt");
	DB.LoadDecks("C:\\Users\\NT\\Documents\\Visual Studio 2008\\Projects\\EvaluateDecks\\bin\\decks\\custom\\custom.txt");
	char buffer[5000];
	DB.GetCustomDecksList(buffer,5000);
	scanf("%c");*/
	/*{
	DB.LoadCardXML("cards.xml");
	DB.LoadMissionXML("missions.xml");
	DB.LoadRaidXML("raids.xml");

	ActiveDeck X("QVDODw+kD5E2+jH8",DB.GetPointer()),Y("RkBvBxB0DNDhDnE0FAFZGrH8IBfZfvvQ",DB.GetPointer());

	printf("X hash: %s\n",X.GetHash64().c_str());
	printf("Y hash: %s\n",Y.GetHash64().c_str());

	Branching B(X,Y);
	while (B.Evaluate());
	scanf("%c");
	}*/
	/*{
	bConsoleOutput = false;

	DB.LoadCardXML("cards.xml");
	DB.LoadMissionXML("missions.xml");
	DB.LoadRaidXML("raids.xml");

	ActiveDeck br4("RkBvBxB0DNDhDnE0FAFZGrH8IBfZfvvQ"
		//"RdAXCCDIDNDdEGEREoFBFHFbGWfogBu4"
		,DB.GetPointer());//mm119("RfBUDWD9E4FNFRFYFdFrFsGnG0frvOvT",DB.GetPointer()); // 
	// base deck:
	ActiveDeck basedeck("QBBj+pBj"
		//"QVCtDODw+kD5E2E2E2"
		//"QVA2CcCtE+E+Fg+jGpGr"
		,DB.GetPointer());
	typedef vector <UINT>	VID;
	typedef set <UINT>		SID;
	VID Commanders;
	VID Cards;
	SID Exclude;
	//Commanders.push_back(DB.CARD("Dracorex")->GetId());
	//Cards.push_back(289);
	//Exclude.insert(289); // DEBUG - EXCLUDE VALEFAR
	if (basedeck.Commander.IsDefined())
		Commanders.push_back(basedeck.Commander.GetId());
	for (UINT icmd=1000;icmd<2000;icmd++)
	{
		Card c = DB.GetCard(icmd);
		if (c.IsCard() && c.GetSet())
		{
			//printf("%d %d	%d	%s\n",c.GetRarity(),c.GetFaction(),icmd,c.GetName());
			if (c.GetRarity() > RARITY_RARE)// debug filter
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
			if ((c.GetRarity() > RARITY_LEGENDARY) 
			//&& (c.GetFaction() == FACTION_IMPERIAL)
			)
				if (Exclude.find(icard) == Exclude.end())
					Cards.push_back(icard);
		}
	}
	//Commanders.push_back(DB.CARD("Dracorex")->GetId());
	//Commanders.push_back(DB.CARD("Ajax")->GetId());
	Cards.push_back(DB.CARD("Valkyrie")->GetId());
	Cards.push_back(DB.CARD("Support Carrier")->GetId());
	//Cards.push_back(DB.CARD("Daemon")->GetId());
	Cards.push_back(DB.CARD("Pummeller")->GetId());
	Cards.push_back(DB.CARD("Aegis")->GetId());
	Cards.push_back(DB.CARD("Apollo")->GetId());
	Cards.push_back(DB.CARD("Fighter Jet")->GetId());
	printf("%d : %d\n",Commanders.size(),Cards.size());
	UINT CardsSize = Cards.size();
#define PICK_DECK_SIZE	10
	UINT Picks[PICK_DECK_SIZE];

#define GAMES_COUNT	100
#define GAMES_EMUL	1000
	float cchance = 0.75; // dont need that crappy decks
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
				}

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
					//ActiveDeck tm(br4);
					ActiveDeck tm;
					DB.GenRaidDeck(tm,5);
					ActiveDeck a(cd);

					Simulate(a,tm,res,false);
				}
				if (res.Win < (2 * GAMES_COUNT * cchance / 3))
					break; // chance is lower than 2/3 of current best, drop this
			}
			float newchance = (float)res.Win / (GAMES_COUNT * GAMES_EMUL);
			if (newchance >= cchance * 0.999) // they are close
			{
				if (newchance > cchance)
					cchance = newchance;
				bestdeck = ActiveDeck(cd);
				printf("[%d] %.3f	%s\n",cardcount,cchance,bestdeck.GetHash64().c_str());
			}
		}
		//printf("%d variations overall\n",cnt);
	}
	printf("Finished");
	scanf("%c");
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
}

