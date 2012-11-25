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


#if 1
    void appendCard(std::stringstream * os
                   ,PlayedCard const & card
                   ,unsigned int const w
                   )
    {
        for(unsigned int i = 0; i < PlayedCard::numberOfCardLines; i++) {
            if(card.IsDefined()) {
                os[i] << std::setw(w) << card.toRectString(w,i);
            } else {
                os[i] << std::setw(w) << "";
            }
        }
    }

    void appendCards(std::stringstream * os, LCARDS const & cards, unsigned int const w)
    {
        for(LCARDS::const_iterator iter = cards.begin()
           ;iter != cards.end()
           ;iter++
        ){
            appendCard(os, *iter, w);
        }
    }

    void concatStreams(std::stringstream & os, std::stringstream const * const oss)
    {
        for(unsigned int i = 0; i < PlayedCard::numberOfCardLines; i++) {
            os << oss[i].str() << std::endl;
        }
    }

    void appendCards(std::stringstream & os, LCARDS const & cards, unsigned int const w)
    {
        std::stringstream oss[PlayedCard::numberOfCardLines];
        appendCards(oss,cards,w);
        concatStreams(os,oss);
    }

    /**
     * Returns a nice string representation of the deck.
     */
    std::string ActiveDeck::toString(bool const & reversed, unsigned int const w)
    {
        std::stringstream ssDeck;
        std::stringstream oss[PlayedCard::numberOfCardLines];

        if(!reversed) {
            appendCards(ssDeck,this->Units,w);
            appendCard(oss,this->Commander,w);
            appendCards(oss,this->Structures,w);
            appendCards(oss,this->Actions,w);
            concatStreams(ssDeck,oss);
        } else {
            appendCard(oss,this->Commander,w);
            appendCards(oss,this->Structures,w);
            appendCards(oss,this->Actions,w);
            concatStreams(ssDeck,oss);
            appendCards(ssDeck,this->Units,w);
        }
        return ssDeck.str();
    }

	string ActiveDeck::GetDeck() const
	{
		if (Deck.empty())
			return string();
		string s;
		char buffer[10];
		if (Commander.IsDefined())
		{
			_itoa_s(Commander.GetId(),buffer,10);
			s.append(buffer);
		}
		for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++) {
			if (!s.empty())
				s.append(",");
			_itoa_s(iter->GetId(),buffer,10);
			s.append(buffer);
		}
		return s;
	}
	string ActiveDeck::GetHash64(bool bCardPicks) const
	{
#define HASH_SAVES_ORDER	1
		if (Deck.empty() && ((!bCardPicks) || (!CardPicks[0])))
			return string();
#if HASH_SAVES_ORDER
		typedef vector<UINT> MSID;
#else
		typedef multiset<UINT> MSID; // I <3 sets, they keep stuff sorted ;)
#endif
		MSID ids;
        if (!bCardPicks) {
            for (LCARDS::const_iterator iter = Deck.begin(); iter != Deck.end(); iter++) {
#if HASH_SAVES_ORDER
                ids.push_back(iter->GetId());
#else
                ids.insert(iter->GetId());
#endif
            }
		}
		else
			for (UCHAR i=0;(i<DEFAULT_DECK_RESERVE_SIZE) && (CardPicks[i]);i++)
				ids.push_back(CardPicks[i]); // multiset is disallowed!
		string s;
		UINT tmp = 0, t;
		unsigned short lastid = 0, cnt = 1;
		if (Commander.IsDefined())
		{
			tmp = ID2BASE64(Commander.GetId());
			s.append((char*)&tmp);
			//printf("1: %s -commander\n",(char*)&tmp);
		}
		MSID::iterator si = ids.begin();
		do
		{
			// we can actually use Id range 4000-4095 (CARD_MAX_ID - 0xFFF) for special codes,
			// adding RLE here
			tmp = ID2BASE64(*si);
			si++;
			if ((lastid != tmp) || (si == ids.end()))
			{
				if (lastid)
				{
					t = lastid;
					if (cnt == 2)
					{
						s.append((char*)&t);
						s.append((char*)&t);
						//printf("4: %s -dupe\n",(char*)&t);
						//printf("4: %s -dupe\n",(char*)&t);

					}
					else
						if (cnt > 2)
						{
							s.append((char*)&t);
							//printf("3: %s -value\n",(char*)&t);
							t = ID2BASE64(CARD_MAX_ID + cnt); // special code, RLE count
							s.append((char*)&t);
							//printf("3: %s -rle\n",(char*)&t);
						}
						else
						{
							s.append((char*)&t);
							//printf("5: %s\n",(char*)&t);
						}
					cnt = 1;
				}
				if (si == ids.end())
				{
					s.append((char*)&tmp);
					//printf("2: %s\n",(char*)&tmp);
				}
				lastid = tmp;
			}
			else
				cnt++;  // RLE, count IDs
		}
		while (si != ids.end());
		return s;
	}
	void ActiveDeck::GetTargets(LCARDS &From, UCHAR TargetFaction, PPCIV &GetTo, bool bForInfuse)
	{
		if (!bForInfuse)
			GetTo.clear();
		UCHAR pos = 0;
		for (LCARDS::iterator vi = From.begin();vi != From.end();vi++)
		{
			if ((vi->IsAlive()) && (((vi->GetFaction() == TargetFaction) && (!bForInfuse)) || (TargetFaction == FACTION_NONE) || ((vi->GetFaction() != TargetFaction) && (bForInfuse))))
				GetTo.push_back(PPCARDINDEX(&(*vi),pos));
			pos++;
		}
	}

    // skipEffects is an array with a solitary 0 as a terminal
    // targetSkills is an array with a solitary 0 as a terminal
	void ActiveDeck::FilterTargets(PPCIV &targets, const EFFECT_ARGUMENT skipEffects[], const EFFECT_ARGUMENT targetSkills[], const int waitMin, const int waitMax, const int attackLimit, bool skipPlayed)
	{
        PPCIV::iterator vi = targets.begin();
        const EFFECT_ARGUMENT* effect;
        bool erase;
		while (vi != targets.end())
		{
            erase = false;
            if(waitMax >= 0 && vi->first->GetWait() > waitMax) {
                erase = true;
            } else if(waitMin > 0 && vi->first->GetWait() < waitMin) {
                // we can ignore a waitMin of 0 since that is the minimum anyway
                erase = true;
            } else if(attackLimit >= 0 && vi->first->GetAttack() < attackLimit) {
                erase = true;
            } else if(skipPlayed && vi->first->GetPlayed()) {
                erase = true;
            } else {
                if(skipEffects != NULL) {
                    for (effect = skipEffects; *effect != 0; ++effect) {
                        assert(*effect < CARD_ABILITIES_MAX); // make sure someone gave us our terminal
                        if(vi->first->GetEffect(*effect)) {
                            erase = true;
                            break;
                        }
                    }
                }

                if(!erase && targetSkills != NULL) {
                    erase = true;
                    for (effect = targetSkills; *effect != 0; ++effect) {
                        assert(*effect < CARD_ABILITIES_MAX); // make sure someone gave us our terminal
                        if(vi->first->GetAbility(*effect)
                            && vi->first->GetAbilityEvent(*effect) == EVENT_EMPTY) {
                            erase = false;
                            break;
                        }
                    }
                }
            }

			if (erase)
				vi = targets.erase(vi);
			else
				vi++; // skip
		}
    }

    // Choose targets up to targetCount.
	void ActiveDeck::RandomizeTarget(PPCIV &targets, UCHAR targetCount, ActiveDeck &Dest, bool canIntercept)
    {
		if ((targetCount != TARGETSCOUNT_ALL) && (!targets.empty()))
		{
    		UCHAR destindex = UCHAR(rand() % targets.size());
			if (canIntercept) {
				destindex = Intercept(targets, destindex, Dest);
            }
			PPCARDINDEX tmp = targets[destindex];
			targets.clear();
			targets.push_back(tmp);
		}
	}
#endif

    /**
     * Hit the commander.
     */
    bool PlayedCard::HitCommander(UINT QuestEffectId
                                 , const UCHAR Dmg
                                 , PlayedCard &Src
                                 , ActiveDeck & ownDeck
                                 , ActiveDeck & otherDeck
                                 , bool bCanBeCountered
                                 , UCHAR *overkill
                                 , VLOG *log
                                 , LOG_RECORD *lr
                                 )
	{
        LCARDS & Structures(ownDeck.Structures);
		assertX(GetType() == TYPE_COMMANDER); // double check for debug

        // 0 dmg is pointless and indicates an error
        if(Dmg <= 0) {
            throw std::invalid_argument("Zero 0 damage in PlayedCard::HitCommander");
        }

		// find a wall to break it ;)
		UCHAR index = 0;
		for (LCARDS::iterator vi = Structures.begin();vi!=Structures.end();vi++)
		{
			if (vi->GetAbility(DEFENSIVE_WALL) && vi->IsAlive())
			{
				if (QuestEffectId != QEFFECT_IMPENETRABLE)
				{
					vi->CardSkillProc(DEFENSIVE_WALL);
					if (lr)
					{
						lr->Target.CardID = index;
						lr->Target.RowID = TYPE_STRUCTURE;
					}



					// walls can counter and regenerate
					vi->SufferDmg(QuestEffectId,Dmg,0,0,0,overkill);

                    // probably here wall's "on attacked" skills
                    assert(vi->IsDefined());
                    assert(Src.IsDefined());
                    ownDeck.ApplyEffects(QuestEffectId,EVENT_ATTACKED,*vi,index,otherDeck,false,false,NULL,0,&Src);

					if (vi->GetAbility(DEFENSIVE_COUNTER) && bCanBeCountered) // counter, dmg from crush can't be countered
					{
						vi->CardSkillProc(DEFENSIVE_COUNTER);
						EFFECT_ARGUMENT cdmg = vi->GetAbility(DEFENSIVE_COUNTER) + Src.GetEffect(ACTIVATION_ENFEEBLE);
						vi->fsDmgDealt += cdmg;
						UCHAR loverkill = 0;
						if (lr && log)
							log->push_back(LOG_RECORD(lr->Target,lr->Src,DEFENSIVE_COUNTER,cdmg));
						Src.SufferDmg(QuestEffectId,cdmg,0,0,0,&loverkill); // counter dmg is enhanced by enfeeble
						vi->fsOverkill += loverkill;
					}
				}
				return false;
			}
			index++;
		}

        // Commander was attacked, trigger event.
        ownDeck.ApplyEffects(QuestEffectId,EVENT_ATTACKED,*this,0,otherDeck,false,false,NULL,0,&Src);

		// no walls found then hit commander
		// ugly - counter procs before commander takes dmg, but whatever
		if (GetAbility(DEFENSIVE_COUNTER) && bCanBeCountered) // commander can counter aswell
		{
			CardSkillProc(DEFENSIVE_COUNTER);
			UCHAR loverkill = 0;
			EFFECT_ARGUMENT cdmg = GetAbility(DEFENSIVE_COUNTER) + Src.GetEffect(ACTIVATION_ENFEEBLE);
			if (lr && log)
				log->push_back(LOG_RECORD(lr->Target,lr->Src,DEFENSIVE_COUNTER,cdmg));
			Src.SufferDmg(QuestEffectId,cdmg,0,0,0,&loverkill); // counter dmg is enhanced by enfeeble
			fsOverkill += loverkill;
		}
		return (SufferDmg(QuestEffectId,Dmg,0,0,0,overkill) > 0);
	}

}}
