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
// contains some threading evaluation routines

#define MAX_FILTER_ID_COUNT		50

CardDB DB; // just to make all easier ...
UINT MaxTurn = MAX_TURN;

// routines
void MergeBuffers(UINT *Dest, const UINT *Src, UINT Size = CARD_ABILITIES_MAX)
{
	_ASSERT(Dest);
	_ASSERT(Src);
	for (UINT i=0;i<Size;i++)
		Dest[i] += Src[i];
}
void MergeBuffers(UINT *Dest, const UCHAR *Src, UINT Size = CARD_ABILITIES_MAX)
{
	_ASSERT(Dest);
	_ASSERT(Src);	
	for (UINT i=0;i<Size;i++)
		Dest[i] += Src[i];
}

void Simulate(ActiveDeck &tAtk, ActiveDeck &tDef, RESULTS &r, const UCHAR *CSIndex = 0, RESULT_BY_CARD *rbc = 0, bool bSurge = false, REQUIREMENT *Reqs = 0, UINT *SkillProcs = 0)
{
/*
Should have given more credit to people that help improving...
Original source is by Hotshot2k4
http://www.kongregate.com/forums/65-tyrant/topics/201617-tyrant-war-scores-formula
but Fonzoland explained it so that I could understand:
All modes.
1. Winning gives a base score of 10 on fight/defense, 30 on surge. Surrender/stall count as a win for the defender.
2. There is a “damage bonus” (all damage dealt to enemy commander, including overkill, in a certain range of turns), which is always capped at 10.
3. There is a “speed bonus” for killing the enemy commander before a certain turn, which is always 5.
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
				if (CSIndex && rbc)
					for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
						if (tAtk.CardPicks[i])
							rbc[CSIndex[tAtk.CardPicks[i]]].PickStats[i].Loss++;

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
						if (xwl.CheckRequirements(Reqs))
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
			if (CSIndex && rbc)
				for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
					if (tAtk.CardPicks[i])
						rbc[CSIndex[tAtk.CardPicks[i]]].PickStats[i].Win++;

			if (tAtk.CheckRequirements(Reqs))
			{
				r.Win++;
				if (SkillProcs)
					MergeBuffers(SkillProcs,tAtk.SkillProcs);
			}
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
				if (CSIndex && rbc)
					for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
						if (tAtk.CardPicks[i])
							rbc[CSIndex[tAtk.CardPicks[i]]].PickStats[i].Loss++;

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
		if (CSIndex && rbc)
			for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
				if (tAtk.CardPicks[i])
					rbc[CSIndex[tAtk.CardPicks[i]]].PickStats[i].Stall++;

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
	REQUIREMENT Req[REQ_MAX_SIZE];
	UINT SkillProcs[CARD_ABILITIES_MAX];
};

void EvaluateRaidOnce(const ActiveDeck gAtk, RESULTS &r, const UCHAR *CSIndex/* = 0*/, RESULT_BY_CARD *rbc/* = 0*/, DWORD RaidID, REQUIREMENT *Reqs = 0, UINT *SkillProcs = 0)
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
			if (CSIndex && rbc)
				for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
					if (tAtk.CardPicks[i])
						rbc[CSIndex[tAtk.CardPicks[i]]].PickStats[i].Win++;

			if (tAtk.CheckRequirements(Reqs))
			{
				r.Win++;
				if (SkillProcs)
					MergeBuffers(SkillProcs,tAtk.SkillProcs);
			}
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
	if (tDef.Commander.IsAlive())
	{
		if (CSIndex && rbc)
			for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
				if (tAtk.CardPicks[i])
				{
					if (!tAtk.Commander.IsAlive())
						rbc[CSIndex[tAtk.CardPicks[i]]].PickStats[i].Loss++;
					else
						rbc[CSIndex[tAtk.CardPicks[i]]].PickStats[i].Stall++;
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
				Simulate(Atk,Def,lr,p->CSIndex,rbc,p->bSurge,p->Req,p->SkillProcs);
			}
			else
				EvaluateRaidOnce(*(p->Atk),lr,p->CSIndex,rbc,(DWORD)p->RaidID,p->Req,p->SkillProcs);
		}
	//_endthread();
	p->r.Add(lr);
	for (UCHAR m=0;m<DEFAULT_DECK_SIZE+1;m++)
		p->rbc[m].Add(rbc[m]);
	return (UINT)p;
}

void EvaluateInThreads(DWORD Seed, const ActiveDeck &gAtk, const ActiveDeck &gDef, int RaidID, RESULTS &ret, RESULT_BY_CARD *rbc, int &State, DWORD gamesperthread, DWORD threadscount = 1, bool bSurge = false, UCHAR TournamentMode = 0, REQUIREMENT *Req = 0, UINT *pSkillProcs = 0)
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
				Simulate(tAtk,tDef,ret,CSIndex,rbc,bSurge,Req,pSkillProcs);
			}
			else
				EvaluateRaidOnce(gAtk,ret,CSIndex,rbc,(DWORD)RaidID,Req,pSkillProcs);
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
			memset(parms[i].SkillProcs,0,sizeof(parms[i].SkillProcs));
			if (Req)
				memcpy(parms[i].Req,Req,sizeof(REQUIREMENT)*REQ_MAX_SIZE);

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

			if (pSkillProcs)
				MergeBuffers(pSkillProcs,parms[i].SkillProcs); // merge these
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
	//
	REQUIREMENT Req[REQ_MAX_SIZE];
	//
	UINT SkillProcs[CARD_ABILITIES_MAX];
};