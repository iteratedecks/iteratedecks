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

#if defined(__linux__) || defined(__MINGW32__)
// for rand
#include <stdlib.h>
#endif

#include <iostream>
#include <sstream>

#include "compat.h"

#include <vector>
#include <list>
#include <set>
#include "ctype.h"
#include <stdexcept>
#include "exceptions.hpp"
#include <iomanip>
#include <assert.h>
#include "assert.hpp"

#include "results.h"
#include "log.h"
#include "Logger.hpp"
#include "cards.hpp"

#include <cstdlib>
#include <cstring>

#include "achievementInfo.hpp"

#include "activeDeck.hpp"

#define FANCY_STATS_COUNT		5

using namespace std;

namespace IterateDecks { namespace Core {
bool bConsoleOutput = false; // this is global just for convinience, should be DEFINE instead, to cleanup the code
int AchievementIndex = -1; // index, not id
}}

#include "deck.hpp"
#include "card.hpp"

namespace IterateDecks { namespace Core {




}}
