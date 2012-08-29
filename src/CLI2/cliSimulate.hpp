#ifndef CLI_SIMULATE_HPP_
    #define CLI_SIMULATE_HPP_

    #include "../EXE/deck.hpp"
    #include "../EXE/Logger.hpp"
    #include "../EXE/results.hpp"

    namespace EvaluateDecks {
        namespace CLI {

            RESULTS simulate(ActiveDeck const & deck1
                             ,ActiveDeck const & deck2
                             ,DeckLogger & attackLogger
                             ,DeckLogger & defenseLogger
                             ,SimulationLogger & simulationLogger
                             ,unsigned int const & numberOfIterations
                             );

        } // namespace CLI
    } // namespace EvaluateDecks
#endif
