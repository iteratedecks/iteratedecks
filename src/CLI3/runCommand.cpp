#include "runCommand.hpp"
#include "../CORE/iterateDecksCore.hpp"

namespace IterateDecks {
    namespace CLI3 {

        RunCommand::RunCommand()
        {
        }

        int RunCommand::execute() {

            IterateDecksCore simulator;

            Result result = simulator.simulate(this->task);

            std::cout << result.gamesWon << " / " << result.numberOfGames << std::endl;
            return 0;
        }
        
    }
}
