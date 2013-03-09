// Warning: Strange charset
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
// this module contains all evaluation related stuff - card and deck classes and their interactions

#include "compat.h"

#include "results.h"
#include "log.h"

#define FANCY_STATS_COUNT		5

using namespace std;

namespace IterateDecks { namespace Core {
bool bConsoleOutput = false; // this is global just for convinience, should be DEFINE instead, to cleanup the code
int AchievementId = -1;
}}
