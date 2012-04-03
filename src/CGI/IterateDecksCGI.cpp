// IterateDecksCGI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\EXE\deck.h"
#include "..\EXE\cards.h"
#include "..\EXE\simulate.h"

// I am not a huge fan of these headers
//#include <iostream>
//#include <conio.h>

// argv[1] - RNG seed
// argv[2] - attacking deck hash
// argv[3] - defending deck hash(doesn't matter for raids)
// argv[4] - attacking deck is ordered(0,1)
// argv[5] - raid id(-1 for non-raid decks)
// argv[6] - games per thread
// argv[7] - thread count

int _tmain(int argc, _TCHAR* argv[])
{
	if (!argc)
		return 1;

	// input parameters:
	DWORD Seed = _wtoi(argv[1]);
	DWORD GamesPerThread = _wtoi(argv[6]);
	DWORD Threads = _wtoi(argv[7]);
	int RaidID = _wtoi(argv[5]);
	bool OrderMatters = (_wtoi(argv[4]) != 0);

	bConsoleOutput = false;
	DB.LoadCardXML("cards.xml");
	DB.LoadRaidXML("raids.xml");

	char AtkDeck[MAX_DECKSTRING_SIZE];
	char DefDeck[MAX_DECKSTRING_SIZE];
	wcstombs(AtkDeck,argv[2],MAX_DECKSTRING_SIZE);
	wcstombs(DefDeck,argv[3],MAX_DECKSTRING_SIZE);
	ActiveDeck X(AtkDeck,DB.GetPointer()),Y(DefDeck,DB.GetPointer());
	X.SetOrderMatters(OrderMatters);

	MaxTurn = 50; // default
	OrderLength = -1;
	int State = 1; // control value >= 0

	RESULTS r;

	EvaluateInThreads(Seed,X,Y,RaidID,r,0,State,GamesPerThread,Threads);

	//cout << r.Win << "/" << r.Loss << "/" << r.Games << endl;
	printf("%d/%d/%d\n",r.Win,r.Loss,r.Games);

	return 0;
}

