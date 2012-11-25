#ifndef RESULTS_HPP_
    #define RESULTS_HPP_

    #include "deck.hpp"

    namespace IterateDecks { namespace Core {

        struct PICK_STATS
{
	DWORD Win;
	DWORD Stall;
	DWORD Loss;
	void Add(const PICK_STATS &a);
	const float Ratio() const;
	bool operator<(const PICK_STATS &ps) const;
};

    struct RESULT_BY_CARD {
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
        DWORD FSOverkill;
        DWORD FSDeaths;
        PICK_STATS PickStats[DEFAULT_DECK_RESERVE_SIZE];

        RESULT_BY_CARD();
        bool IsValid();
        void Print();
        void Add(const RESULT_BY_CARD &rAdd);
    };


    struct RESULTS
    {
        DWORD Win;
        DWORD Loss;
        DWORD Games;
        DWORD Points;
        DWORD AutoPoints;
        DWORD LPoints;
        DWORD LAutoPoints;
        RESULTS();
        void Add(const RESULTS rAdd);
    };

}}
#endif
