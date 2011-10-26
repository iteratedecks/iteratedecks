// this types are integrated very poorly ;(
// I can't think of a better way at the moment
typedef vector<ActiveDeck> VAD;
struct BranchParams
{
public:
	VAD *pAtk;
	VAD *pDef;
	UINT Size;
	BranchParams() { Size = 0; }
	BranchParams(VAD *patk, VAD *pdef)
	{
		pAtk = patk;
		pDef = pdef;
		_ASSERT(pAtk);
		Size = pAtk->size();
	}
};

// this class should build all possible games for a pair of decks
// branch should apper on every PROC and on every target filter
// also, there should be some special branch for hand (3 cards)
// and pick of card from that hand, it's a lot of branches, but
// this will remove any inaccuracy that is in random executions now,
// should have started from this, this will give the exact win ratio
// and is a framework for AI
// I think I will integrate it into ActiveDecks by pointers to VAD...
// because there is too many procs
#include "BranchParams.h"
class Branching
{
	VAD Atk;
	VAD Def;
	bool bSurge; // mode
	RESULTS r;
public:
	Branching(ActiveDeck &atk, ActiveDeck &def, bool Surge = false)
	{
		Atk.clear();
		Def.clear();
		Atk.push_back(atk);
		Def.push_back(def);
		bSurge = Surge;
	}
	bool Evaluate()
	{
		_ASSERT(Atk.size() == Def.size()); // should be equal
		BranchParams Params;
		UINT cnt = Atk.size();
		Params.Size = cnt;
		for (UINT i = 0; i < cnt; i++)
		{
			if (bSurge)
			{
				Params.pAtk = &Def;
				Params.pDef = &Atk;
				Def[i].AttackDeck(Atk[i], (ActiveDeck::BranchParams*)&Params);
				if (!Atk[i].Commander.IsAlive())
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
			Params.pAtk = &Atk;
			Params.pDef = &Def;
			Atk[i].AttackDeck(Def[i], (ActiveDeck::BranchParams*)&Params);
			if (!Def[i].Commander.IsAlive())
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
				Params.pAtk = &Def;
				Params.pDef = &Atk;
				Def[i].AttackDeck(Atk[i], (ActiveDeck::BranchParams*)&Params);
				if (!Atk[i].Commander.IsAlive())
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
	}
	// I should erase some of the branch elements here ...
	/*  THIS SHOULD GO AFTER ALL 50 TURNS
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
	*/
};