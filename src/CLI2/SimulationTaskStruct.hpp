#ifndef ITERATEDECKS_CLI_SIMULATIONTASKSTRUCT_HPP
    #define ITERATEDECKS_CLI_SIMULATIONTASKSTRUCT_HPP

    #include "cliOptions.hpp"
    #include "../CORE/iterateDecksCore.hpp"

    namespace IterateDecks {
        namespace CLI {

            struct SimulationTaskStruct {
                unsigned long minimalNumberOfGames;
                bool surge;
                bool delayFirstAttacker; //< live battles
                BattleGroundEffect battleGround; //< for quests and conquest
                AchievementOptions achievementOptions;
                unsigned int randomSeed;
                bool allowInvalidDecks;
                
                DeckArgument attacker;
                DeckArgument defender;

                IterateDecks::Core::SimulationTaskClass toSimulationTaskClass(CardDB const & cardDB);
            };

        }
    }
#endif
