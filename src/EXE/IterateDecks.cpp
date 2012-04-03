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

#include <tchar.h>

#include "stdio.h"
#include "time.h"

#include "deck.h"

#include "cards.h"

#include "branches.h"

#include "simulate.h"

#ifdef _DEBUG
#include <direct.h>
#endif

int _tmain(int argc, char* argv[])
{
/*
	ActiveDeck m154("SGJDJRJbJZDhGWH0IMI6gz",DB.GetPointer());
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
					ActiveDeck tm(m154);
					ActiveDeck a(DB.CARD("Lord of Tartarus"));		
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
	return 1;*/
/*
	bConsoleOutput = false;
	DB.LoadCardXML("cards.xml");
	DB.LoadRaidXML("raids.xml");
	
	ActiveDeck br4;//mm119("RfBUDWD9E4FNFRFYFdFrFsGnG0frvOvT",DB.GetPointer()); // 
	// base deck:
	ActiveDeck basedeck;
	typedef vector <UINT>	VID;
	typedef set <UINT>		SID;
	VID Commanders;
	VID Cards;
	SID Exclude;
	Commanders.push_back(DB.CARD("Dracorex")->GetId());
	// Pantheon Perfect
	Cards.push_back(DB.CARD("Utopia Beacon")->GetId());
	Cards.push_back(DB.CARD("Noxious Den")->GetId());
	Cards.push_back(DB.CARD("Earthquake Generator")->GetId());
	Cards.push_back(DB.CARD("Bridge of Destiny")->GetId());
	Cards.push_back(DB.CARD("Engulfing Chasm")->GetId());
	//Cards.push_back(DB.CARD("Abominable Horror")->GetId());
	//Cards.push_back(DB.CARD("Hephatat")->GetId());
	Cards.push_back(DB.CARD("Support Carrier")->GetId());
	//Cards.push_back(DB.CARD("Deadblow")->GetId());
	Cards.push_back(DB.CARD("Apollo")->GetId());
	//Cards.push_back(DB.CARD("Bolide Walker")->GetId());
	//Cards.push_back(DB.CARD("Abominable Raksha")->GetId());
	//Cards.push_back(DB.CARD("Elemental")->GetId());
	//Cards.push_back(DB.CARD("Lance Rider")->GetId());
	//Cards.push_back(DB.CARD("Arms Depot")->GetId());
	Cards.push_back(DB.CARD("Scepter")->GetId());
	//Cards.push_back(DB.CARD("Lurking Carrier")->GetId());	

	//Exclude.insert(289); // DEBUG - EXCLUDE VALEFAR
	if (basedeck.Commander.IsDefined())
		Commanders.push_back(basedeck.Commander.GetId());

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
			if (Picks[0] > CardsSize)
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

			if (cardcount < 9) // check only full decks
				continue;

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
					DB.GenRaidDeck(tm,13);
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
	scanf("%c");*/


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

	if (pEvalParams->OrderHeapSize > 0)
	{
		MaxOrderHeapSize = pEvalParams->OrderHeapSize;
		OrderLength = DEFAULT_DECK_SIZE;
	}
	else
		OrderLength = -1;

	memset(&pEvalParams->Result,0,sizeof(RESULTS));
	memset(&pEvalParams->ResultByCard,0,sizeof(RESULT_BY_CARD) * (DEFAULT_DECK_RESERVE_SIZE + 1));	
	memset(&pEvalParams->SkillProcs,0,sizeof(UINT)*CARD_ABILITIES_MAX);
	memset(&pEvalParams->ResultByOrder,0,sizeof(RESULT_BY_ORDER)*DEFAULT_DECK_RESERVE_SIZE);
	memset(&pEvalParams->WildCardIds,0,sizeof(UCHAR)*DEFAULT_DECK_RESERVE_SIZE);
	memset(&pEvalParams->WildCardWins,0,sizeof(UINT)*DEFAULT_DECK_RESERVE_SIZE);
	pEvalParams->FullAmountOfGames = 0;

	time_t t;
	time(&t);
	if (pEvalParams->WildcardId)
	{
		SCID CardPool;
		SCID xCardPool;
		struct more
		{
		  bool operator()(const UINT a, const UINT b) const
		  {
			return a > b;
		  }
		};
		typedef map<UINT,UINT, more> MRID;
		typedef pair<UINT,UINT> PRID;
		MRID RID;
		if (pEvalParams->WildFilterSpecific)
		{
			DB.LoadCardList("wildcard\\include.txt",CardPool);
			DB.LoadCardList("wildcard\\exclude.txt",xCardPool);
		}
		MUUINT OwnedCards;
		if (pEvalParams->SkipWildCardsWeDontHave)
		{
			DB.LoadOwnedCards("wildcard\\ownedcards.txt",OwnedCards);
			/*for (MUUINT::iterator mi = OwnedCards.begin();mi!=OwnedCards.end();mi++)
			{
				printf("%d : %d\n",mi->first,mi->second);
			}*/
			// mark all cards in deck as used, except for one wildcard
			bool bWildMarked = false;
			for (VCARDS::iterator vi=X.Deck.begin();vi!=X.Deck.end();vi++)
				if ((vi->GetId() == pEvalParams->WildcardId) && (!bWildMarked))
					bWildMarked = true;	// skip only one wildcard
				else
				{
					MUUINT::iterator mi = OwnedCards.find(vi->GetId());
					if (mi != OwnedCards.end())
					{
						mi->second--; // don't check whether we have enough cards or not
					}
					// else DON'T CHANGE ANYTHING IF WE DON'T HAVE BASIC CARDS
				}
		}
		if (pEvalParams->WildcardId < 0)
		{
			// remove all non-commander cards from pool
			for (SCID::iterator si = CardPool.begin(); si != CardPool.end(); si++)
			{
				while ((si != CardPool.end()) && (DB.GetCard(*si).GetType() != TYPE_COMMANDER))
					si = CardPool.erase(si);
				if (si == CardPool.end())
					break;
			}
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
			CardPool.insert(pEvalParams->WildcardId); // include original card
			// remove all commander cards from pool
			for (SCID::iterator si = CardPool.begin(); si != CardPool.end(); si++)
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
		for (SCID::iterator xi = xCardPool.begin(); xi != xCardPool.end(); xi++)
		{
			SCID::iterator si = CardPool.find(*xi);
			if (si != CardPool.end())
				CardPool.erase(si);
		}
		if (CardPool.empty())
			pEvalParams->WildcardId = -1; // tell API that there were no cards in filter

		UINT BestCard = 0;
		for (SCID::iterator si=CardPool.begin();si!=CardPool.end();si++)
		{
			if (pEvalParams->SkipWildCardsWeDontHave && (!OwnedCards.empty()))
			{
				MUUINT::iterator mi = OwnedCards.find(*si);
				if (mi != OwnedCards.end())
				{
					if (mi->second < 1)
						continue; // we don't have enough cards of a type
				}
				else continue; // we don't have this card
			}
			ActiveDeck x(X);
			if (pEvalParams->WildcardId < 0)
				x.Commander = PlayedCard(&DB.GetCard(*si));
			else
				x.Deck.push_back(&DB.GetCard(*si));
			if (!x.IsValid())
				continue;
			RESULTS lresult;
			RESULT_BY_CARD lrbc[DEFAULT_DECK_RESERVE_SIZE+1];
			EvaluateInThreads(pEvalParams->Seed,x,Y,pEvalParams->RaidID,lresult,lrbc,pEvalParams->State,pEvalParams->GamesPerThread,pEvalParams->Threads,pEvalParams->Surge,pEvalParams->TournamentMode,pEvalParams->Req,pEvalParams->SkillProcs,pEvalParams->Annihilator,pEvalParams->SurrenderAtLoss);
			if (lresult.Win > pEvalParams->Result.Win)
			{
				pEvalParams->Result = lresult;
				memcpy(pEvalParams->ResultByCard,lrbc,sizeof(lrbc));
				BestCard = *si;
			}
			RID.insert(PRID(lresult.Win,*si));
			pEvalParams->FullAmountOfGames += lresult.Games;
		}
		if (BestCard)
			pEvalParams->WildcardId = BestCard;
		UCHAR i = 0;
		for (MRID::iterator mi = RID.begin(); (mi != RID.end()) && (i < DEFAULT_DECK_RESERVE_SIZE /* amount of wildcards to dump */); mi++)
			if (mi->second != BestCard)
			{
				pEvalParams->WildCardWins[i] = mi->first;
				pEvalParams->WildCardIds[i] = mi->second;
				i++;
			}
	}
	else
	{
		// simple eval
		EvaluateInThreads(pEvalParams->Seed,X,Y,pEvalParams->RaidID,pEvalParams->Result,pEvalParams->ResultByCard,pEvalParams->State,pEvalParams->GamesPerThread,pEvalParams->Threads,pEvalParams->Surge,pEvalParams->TournamentMode,pEvalParams->Req,pEvalParams->SkillProcs,pEvalParams->Annihilator,pEvalParams->SurrenderAtLoss);
		pEvalParams->FullAmountOfGames += pEvalParams->Result.Games;
	}
	if (OrderLength > 0)
	{
		// reorder
		typedef map<PICK_STATS, string> MPSS;
		typedef pair<PICK_STATS, string> PAIRPSS;
		MPSS OrderByStats;
		for (MSPS::iterator mi = StatsByOrder.begin(); mi != StatsByOrder.end(); mi++)
			OrderByStats.insert(PAIRPSS(mi->second,mi->first));
		UCHAR k=0;
		for (MPSS::iterator mi = OrderByStats.end();;mi--)
		{
			if (mi != OrderByStats.end())
			{
				strcpy(pEvalParams->ResultByOrder[k].CardOrder,mi->second.c_str());
				pEvalParams->ResultByOrder[k].result = mi->first;

				ActiveDeck x(mi->second.c_str(),DB.GetPointer());
				for (UCHAR i=0;i<x.Deck.size();i++)
					pEvalParams->ResultByOrder[k].CardIDs[i] = x.Deck[i].GetId(); 
				//printf("%3d: %s	%.3f	%d \\ %d \\ %d\n",k,mi->second.c_str(),mi->first.Ratio(),mi->first.Win,mi->first.Stall,mi->first.Loss);
			}
			else
				if (k)
					break;
			k++;
			if (k > 10)
				break;
		}
		OrderByStats.clear();
		StatsByOrder.clear();
	}
	time_t t1;
	time(&t1);
	pEvalParams->Seconds = (DWORD)t1-t;
	printf("Finished in %d sec\n",t1-t);

	UnmapViewOfFile(pEvalParams);
	CloseHandle(hFileMapping);

   return 1;
#else
	bConsoleOutput = false;
	DB.LoadCardXML("C:\\Users\\NT\\Documents\\Visual Studio 2008\\Projects\\EvaluateDecks\\bin\\cards.xml");
	DB.LoadMissionXML("missions.xml");
	DB.LoadRaidXML("raids.xml");
	//DB.SaveMissionDecks("c:\\pun.txt");
	ActiveDeck x("ROgvgfIf+nIf",DB.GetPointer());
	ActiveDeck z("SQJ6FCH/G0I/JCDdJJI2HMJEGsgofou8",DB.GetPointer());
	x.SetOrderMatters(true);

	//DB.LoadDecks("C:\\Users\\NT\\Documents\\Visual Studio 2008\\Projects\\EvaluateDecks\\bin\\decks\\batcheval\\New Missions.txt");

	bConsoleOutput = true;
	{
	RESULTS r;
	for (UINT k=0;k<1000;k++)
	{
		ActiveDeck X(x);
		ActiveDeck Y(z);

		printf("-----------------------------------\n");
		Simulate(X,Y,r);
		if (!X.Commander.IsAlive())
		{
			printf("Lost!\n");
		}
	}
	return r.Win;
	}

	{
	RESULTS r;
	ActiveDeck Y;
	ActiveDeck X("QefWBD",DB.GetPointer());
	Y.SetOrderMatters(true);
	DB.GenRaidDeck(Y,1);
	//DB.LoadDecks("C:\\Users\\NT\\Documents\\Visual Studio 2008\\Projects\\EvaluateDecks\\bin\\decks\\_defence\\crash.txt");
	//DB.GetCustomDeck(0,Y);
	Simulate(X,Y,r);
	}
	
	//printf("%d %d %d\n",sizeof(PICK_STATS),sizeof(RESULT_BY_CARD),sizeof(EVAL_PARAMS));
	//DB.CreateDeck("Gaia, Utopia Beacon(3), Support Carrier(2), Flux Blaster, Radiant Dawnbringer, Acropolis, Adytum(2)",x);
	//DB.CreateDeck("Krellus[1144],Abominable Raksha,Venorax,Beetle Bomber,Phantom,Azure Reaper,Xeno Mothership,Xeno Overlord,Cloaked Exarch,Kyrios,Revoker,Predator,Shaded Hollow,Vaporwing,Landing Pods,Chaos Wave",z);
	
	VLOG log;
	log.reserve(10000);

	RESULTS r;
	RESULT_BY_CARD rbc[DEFAULT_DECK_RESERVE_SIZE+1];
	UCHAR CSIndex[CARD_MAX_ID];
	memset(CSIndex,0,sizeof(CSIndex));
	CSIndex[x.Commander.GetId()] = 0;
	rbc[0].Id = x.Commander.GetId();
	for (UCHAR i=0;i<x.Deck.size();i++)
	{
		UCHAR idx = 0;
		for (idx=0;idx<DEFAULT_DECK_RESERVE_SIZE+1;idx++)
			if (rbc[idx].Id == x.Deck[i].GetId())
				break;
			else
				if (!rbc[idx].IsValid())
				{
					rbc[idx].Id = i;
					break;
				}
		_ASSERT(idx);
		CSIndex[x.Deck[i].GetId()] = idx;
		rbc[idx].Id = x.Deck[i].GetId();
	}
	{
	RESULTS r;
	for (UINT k=0;k<1000;k++)
	{
		ActiveDeck X(x);
		ActiveDeck Y(z);

		Simulate(X,Y,r,CSIndex,rbc);
	}
	return r.Win;
	}
	//EvaluateRaidOnce(x,r,0,0,12);
	for (UINT k=0;k<1000;k++)
	{
		ActiveDeck a(x),b(z);
		a.Log = &log;
		a.LogDeckID = 0;
		b.Log = &log;
		b.LogDeckID = 1;
		Simulate(a,b,r,CSIndex,rbc);
		//
		// print log
		char death[] = "dies";
		char attack[] = "attacks";
		for (UINT i=0;i<log.size();i++)
		{
			char *aname;
			if (!log[i].AbilityID)
				if (!log[i].Target.IsValid())
					aname = death;
				else
					aname = attack;
			else
				aname = DB.Skills[log[i].AbilityID].SkillName;
			if (log[i].Target.IsValid())
				printf("%d: %2d,%2d - %s -> %d : %2d,%2d = %d\n",
					log[i].Src.DeckID,log[i].Src.CardID,log[i].Src.RowID,
					aname,
					log[i].Target.DeckID,log[i].Target.CardID,log[i].Target.RowID,
					log[i].Effect);
			else
				printf("%d: %2d,%2d - %s -> = %d\n",
					log[i].Src.DeckID,log[i].Src.CardID,log[i].Src.RowID,
					aname,
					log[i].Effect);
		}
		printf("---------------------------------\n");
	}
	printf("%d\n",r.Win);
	for (UCHAR i=0;i<DEFAULT_DECK_SIZE;i++)
	{
		if (!rbc[i].Id) continue;
		printf("%s:\n",DB.GetCard(rbc[i].Id).GetName());
		if (DB.GetCard(rbc[i].Id).GetType() == TYPE_COMMANDER) continue;
		for (UCHAR k=0;k<DEFAULT_DECK_SIZE;k++)
			printf("   %d \\ %d \\ %d -> %.2f\n",rbc[i].PickStats[k].Win,rbc[i].PickStats[k].Stall,rbc[i].PickStats[k].Loss,(float)rbc[i].PickStats[k].Win / (rbc[i].PickStats[k].Win+rbc[i].PickStats[k].Stall+rbc[i].PickStats[k].Loss));
	}

	int c = 0;
	float bestratio = 0.0;
	printf("Heap size: %d Ratio: %.3f\n",StatsByOrder.size(),(float)r.Win/r.Games);
	for (map <string, PICK_STATS> :: iterator mi = StatsByOrder.begin(); mi != StatsByOrder.end(); mi++)
	{
		float ratio = (float)mi->second.Win / (mi->second.Win + mi->second.Stall + mi->second.Loss);
		if (((float)r.Win/r.Games <= ratio) && (bestratio <= ratio))
		{
			printf("%3d: %s	%.3f	%d \\ %d \\ %d\n",c,mi->first.c_str(),ratio,mi->second.Win,mi->second.Stall,mi->second.Loss);
			bestratio = ratio;
			c++;
		}
	}
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

				//вычисление вектора невязки приближения x
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

			//вывод решения
			for (k=0; k<PARAMS_COUNT; k++)
				printf("x%d = %.2f\n",k,x[k]);
				//cout<<"x"<<k<<" = "<<x[k]<<endl;

			//вывод вектора невязки
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
			//вывод количества потребовавшихся итераций
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

