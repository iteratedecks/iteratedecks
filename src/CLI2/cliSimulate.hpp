#ifndef CLI_SIMULATE_HPP_
    #define CLI_SIMULATE_HPP_

    #include "../EXE/deck.hpp"
    #include "../EXE/Logger.hpp"
    #include "../EXE/results.hpp"

    namespace EvaluateDecks {
        namespace CLI {


            /**
             * Compute centered two sided confidence interval.
             */
            void twoSidedBounds(unsigned int const k    //< wins
                               ,unsigned int const n    //< games
                               ,double const gamma      //< confidence level
                               ,double & lower          //< output: lower bound
                               ,double & upperBound     //< output: upper bound
                               );
            
            /**
             * p, such that B(X >= k   | p,n ) = 1-gamma
             * p, such that B(X <  k   | p,n ) = gamma
             * p, such that B(X <= k-1 | p,n ) = gamma
             * where B is the Binomial distribution.
             */
            double oneSidedLowerBound(unsigned int const k  //< wins
                                     ,unsigned int const n  //< games
                                     ,double const gamma    //< confidence level
                                     );

            /**
             * p, such that B(X <= k | p,n ) = 1-gamma
             * where B is the Binomial distribution.
             */
            double oneSidedUpperBound(unsigned int const k  //< successfull results
                                     ,unsigned int const n  //< total tries
                                     ,double const gamma    //< confidence level
                                     );

            RESULTS simulate(ActiveDeck const & deck1
                             ,ActiveDeck const & deck2
                             ,DeckLogger & attackLogger
                             ,DeckLogger & defenseLogger
                             ,SimulationLogger & simulationLogger
                             ,unsigned int const & numberOfIterations
                             ,bool surge = false
                             );

            RESULTS simulateRaid(ActiveDeck const & deck1
                                , unsigned int const & raidId
                                , DeckLogger & attackLogger
                                , SimulationLogger & simulationLogger
                                , unsigned int const & numberOfIterations
                                );

            RESULTS simulateQuest(ActiveDeck const & deck1
                                , unsigned int const & questId
                                , DeckLogger & attackLogger
                                , SimulationLogger & simulationLogger
                                , unsigned int const & numberOfIterations
                                );

            void printResults(RESULTS r);

        } // namespace CLI
    } // namespace EvaluateDecks
#endif
