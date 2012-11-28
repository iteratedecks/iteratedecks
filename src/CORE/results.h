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
// this module contains interfaces used by CORE and probably others to exchange evaluation results
// this structure is used to store and return simulation result

#if !defined(_MSC_VER)
// contains memset
#include <string.h>
#endif

#include "deck.hpp"
#include "results.hpp"
#include "assert.hpp"

namespace IterateDecks { namespace Core {

// struct RESULTS
	RESULTS::RESULTS()
	{
		Win = 0;
		Loss = 0;
		Games = 0;
		Points = 0;
		AutoPoints = 0;
		LPoints = 0;
		LAutoPoints = 0;
	}
	void RESULTS::Add(const RESULTS rAdd)
	{
		Win += rAdd.Win;
		Loss += rAdd.Loss;
		Games += rAdd.Games;
		Points += rAdd.Points;
		AutoPoints += rAdd.AutoPoints;
		LPoints += rAdd.LPoints;
		LAutoPoints += rAdd.LAutoPoints;
	}

#if 1
	void PICK_STATS::Add(const PICK_STATS &a)
	{
		Win += a.Win;
		Stall += a.Stall;
		Loss += a.Loss;
	}
	const float PICK_STATS::Ratio() const
	{
		return (float)Win / (Win + Stall + Loss);
	}
	bool PICK_STATS::operator<(const PICK_STATS &ps) const
	{
		if (Ratio() < ps.Ratio())
			return true;
		else
			if (Ratio() > ps.Ratio())
				return false;
			else
				if (Win < ps.Win)
					return true;
				else
					if (Win > ps.Win)
						return false;
					else
						return (Stall < ps.Stall);
	}
#endif

struct RESULT_BY_ORDER
{
	char CardOrder[DEFAULT_DECK_RESERVE_SIZE * 2];
	UINT CardIDs[DEFAULT_DECK_RESERVE_SIZE];
	PICK_STATS result;
};

#if 1
	RESULT_BY_CARD::RESULT_BY_CARD()
	{
		Id = 0;
		WLGames = 0;
		WLWin = 0;
		WLLoss = 0;
		FSRecordCount = 0;
		FSMitigated = 0;
		FSAvoided = 0;
		FSDamage = 0;
		FSHealing = 0;
		FSSpecial = 0;
		FSOverkill = 0;
		FSDeaths = 0;
		memset(PickStats,0,sizeof(PICK_STATS)*DEFAULT_DECK_RESERVE_SIZE);
	}
	bool RESULT_BY_CARD::IsValid()
	{
		return Id != 0;
	}
	void RESULT_BY_CARD::Print()
	{
		if (!IsValid())
			return;
		printf("CardID %4ld:\n",Id);
		if (WLGames)
			printf("G%ld	W%ld	L%ld:\n",WLGames,WLWin,WLLoss);
		if (!FSRecordCount)
			return;
		printf("	Records		%5ld:\n",FSRecordCount);
		printf("	Avoided:	%5ld	| %5.2f per card per game\n",FSAvoided,(float)FSAvoided / FSRecordCount);
		printf("	Dealt:		%5ld	| %5.2f per card per game\n",FSDamage,(float)FSDamage / FSRecordCount);
		printf("	Mitigated:	%5ld	| %5.2f per card per game\n",FSMitigated,(float)FSMitigated / FSRecordCount);
		printf("	Healed:		%5ld	| %5.2f per card per game\n",FSHealing,(float)FSHealing / FSRecordCount);
		printf("	Special:	%5ld	| %5.2f per card per game\n",FSSpecial,(float)FSSpecial / FSRecordCount);
	}
	void RESULT_BY_CARD::Add(const RESULT_BY_CARD &rAdd)
	{
		assertX(Id == rAdd.Id);
		WLGames += rAdd.WLGames;
		WLWin += rAdd.WLWin;
		WLLoss += rAdd.WLLoss;
		FSRecordCount += rAdd.FSRecordCount;
		FSMitigated += rAdd.FSMitigated;
		FSAvoided += rAdd.FSAvoided;
		FSDamage += rAdd.FSDamage;
		FSHealing += rAdd.FSHealing;
		FSSpecial += rAdd.FSSpecial;
		FSOverkill += rAdd.FSOverkill;
		FSDeaths += rAdd.FSDeaths;
		for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
			PickStats[i].Add(rAdd.PickStats[i]);
	}
#endif

}}
