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
// this module contains interfaces used by API, DLL and EXE to exchange evaluation results 
// this structure is used to store and return simulation result

typedef unsigned long       DWORD;
struct RESULTS
{
	DWORD Win;
	DWORD Loss;
	DWORD Games;
	DWORD Points;
	DWORD AutoPoints;
	DWORD LPoints;
	DWORD LAutoPoints;
	RESULTS()
	{
		Win = 0;
		Loss = 0;
		Games = 0;
		Points = 0;
		AutoPoints = 0;
		LPoints = 0;
		LAutoPoints = 0;
	}
	void Add(const RESULTS rAdd)
	{
		Win += rAdd.Win;
		Loss += rAdd.Loss;
		Games += rAdd.Games;
		Points += rAdd.Points;
		AutoPoints += rAdd.AutoPoints;
		LPoints += rAdd.LPoints;
		LAutoPoints += rAdd.LAutoPoints;
	}
};
//
struct PICK_STATS
{
	DWORD Win;
	DWORD Stall;
	DWORD Loss;
	void Add(const PICK_STATS &a)
	{
		Win += a.Win;
		Stall += a.Stall;
		Loss += a.Loss;
	}
};
struct RESULT_BY_CARD
{
	DWORD Id;
	DWORD WLGames;
	DWORD WLWin;
	DWORD WLLoss;
	DWORD FSRecordCount;
	DWORD FSMitigated;
	DWORD FSAvoided;
	DWORD FSDamage;
	DWORD FSHealing;
	DWORD FSSpecial;
	PICK_STATS PickStats[DEFAULT_DECK_RESERVE_SIZE];
	RESULT_BY_CARD()
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
		memset(PickStats,0,sizeof(PICK_STATS)*DEFAULT_DECK_RESERVE_SIZE);
	};
	bool IsValid()
	{
		return Id != 0;
	};
	void Print()
	{
		if (!IsValid())
			return;
		printf("CardID %4d:\n",Id);
		if (WLGames)
			printf("G%d	W%d	L%d:\n",WLGames,WLWin,WLLoss);
		if (!FSRecordCount)
			return;
		printf("	Records		%5d:\n",FSRecordCount);
		printf("	Avoided:	%5d	| %5.2f per card per game\n",FSAvoided,(float)FSAvoided / FSRecordCount);
		printf("	Dealt:		%5d	| %5.2f per card per game\n",FSDamage,(float)FSDamage / FSRecordCount);
		printf("	Mitigated:	%5d	| %5.2f per card per game\n",FSMitigated,(float)FSMitigated / FSRecordCount);
		printf("	Healed:		%5d	| %5.2f per card per game\n",FSHealing,(float)FSHealing / FSRecordCount);
		printf("	Special:	%5d	| %5.2f per card per game\n",FSSpecial,(float)FSSpecial / FSRecordCount);
	}
	void Add(const RESULT_BY_CARD &rAdd)
	{
		_ASSERT(Id == rAdd.Id);
		WLGames += rAdd.WLGames;
		WLWin += rAdd.WLWin;
		WLLoss += rAdd.WLLoss;
		FSRecordCount += rAdd.FSRecordCount;
		FSMitigated += rAdd.FSMitigated;
		FSAvoided += rAdd.FSAvoided;
		FSDamage += rAdd.FSDamage;
		FSHealing += rAdd.FSHealing;
		FSSpecial += rAdd.FSSpecial;
		for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
			PickStats[i].Add(rAdd.PickStats[i]);
	}
};