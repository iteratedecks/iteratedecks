#include <stddef.h>

namespace IterateDecks { namespace Core {

void Simulate(ActiveDeck &tAtk
             ,ActiveDeck &tDef
             ,RESULTS &r
             ,SimulationLogger * logger = NULL
             ,const UCHAR *CSIndex = NULL
             ,RESULT_BY_CARD *rbc = NULL
             ,bool bSurge = false
             ,REQUIREMENT *Reqs = 0
             ,UINT *SkillProcs = 0
             ,bool bAnnihilator = false
             ,bool bSurrenderAtLoss = false
             );

void EvaluateRaidQuestOnce(const ActiveDeck gAtk
                           , RESULTS &r
                           , const UCHAR *CSIndex/* = 0*/
                           , RESULT_BY_CARD *rbc/* = 0*/
                           , DWORD RaidID
                           , DWORD QuestID
                           , REQUIREMENT *Reqs = 0
                           , UINT *SkillProcs = 0
                           , bool bAnnihilator = false
                           , bool bSurrenderAtLoss = false);

}}
