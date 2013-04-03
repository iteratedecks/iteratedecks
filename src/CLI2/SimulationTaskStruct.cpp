#include "SimulationTaskStruct.hpp"
#include "deckTemplates.hpp"
namespace IterateDecks {
    namespace CLI {

        IterateDecks::Core::SimulationTaskClass SimulationTaskStruct::toSimulationTaskClass(CardDB const & cardDB)
        {
            IterateDecks::Core::SimulationTaskClass taskClass;
            taskClass.minimalNumberOfGames = this->minimalNumberOfGames;
            taskClass.surge = this->surge;
            taskClass.delayFirstAttacker = this->delayFirstAttacker;
            taskClass.battleGround = this->battleGround;
            taskClass.achievementOptions = this->achievementOptions;
            taskClass.randomSeed = this->randomSeed;

            taskClass.attacker = createDeckTemplate(this->attacker, cardDB);
            taskClass.attacker->allowInvalid = this->allowInvalidDecks;
            taskClass.defender = createDeckTemplate(this->defender, cardDB);
            taskClass.defender->allowInvalid = this->allowInvalidDecks;

            return taskClass;
        }

    }
}
