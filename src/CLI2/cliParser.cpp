/**
 * Copyright 2012 Fabian "Praetorian" Kürten.
 *
 * This file is part of EvaluateDecks.
 *
 *  EvaluateDecks is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, only version 3 of the License.
 *
 *  EvaluateDecks is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with EvaluateDecks.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  In addition this file can be redistributed and/or modified under
 *  version 3 of the GNU General Public License or any later version
 *  of the License.
 */

#include "cliParser.hpp"
#include "../EXE/exceptions.hpp"

#include <getopt.h>
#include <sstream>
#include <ctime>

namespace EvaluateDecks {
    namespace CLI {

        /**
         * Option table
         */
        static option const long_options[] =
            { { "number-of-iterations" , required_argument, 0, 'n' }
            , { "first-deck-is-ordered", no_argument      , 0, 'o' }
            , { "achievement-index"    , required_argument, 0, 'a' }
            , { "verify"               , required_argument, 0 , 0 }
            , { "verbose"              , no_argument      , 0 , 'v' }
            , { "seed"                 , optional_argument, 0 , 0 }
            , { "color"                , optional_argument, 0 , 0 }
            , { "help"                 , no_argument      , 0 , 'h' }
            };
        static char const * const short_options = "n:oa:v";

        CliOptions parseCliOptions(int const & argc, char * const argv[]) throw (std::invalid_argument, std::logic_error)
        {
            CliOptions options;
            // gnu getopt stuff
            while(true) {
                int option_index = 0;
                int c = getopt_long(argc, argv, short_options, long_options, &option_index);
                if(c == -1) {
                    break;
                }
          
                switch(c) {
                    case 'n': {
                            std::stringstream ssNumberOfIterations(optarg);
                            ssNumberOfIterations >> options.numberOfIterations;
                            if(ssNumberOfIterations.fail()) {
                                throw std::invalid_argument("-n --number-of-iterations requires an integer argument");
                            }
                        } break;
                    case 'o': {
                            if (options.attackDeck.getType() == DeckArgument::HASH) {
                                options.attackDeck.setOrdered(true);
                            } else {
                                throw std::invalid_argument("ordered deck only makes sense for hash decks");
                            }
                        } break;
                    case 'a': {
                            std::stringstream ssAchievementIndex(optarg);
                            int achievementIndex;
                            ssAchievementIndex >> achievementIndex;
                            if(ssAchievementIndex.fail()) {
                                throw std::invalid_argument ("-a --achievement-index requires an integer argument");
                            }
                            if (achievementIndex >= 0) {
                                options.achievementOptions.enableCheck(achievementIndex);
                            } else {
                                options.achievementOptions.disableCheck();
                            }
                        } break;
                    case 'v': {
                            options.verbosity++;
                        } break;
                    case 'h': {
                            options.printHelpAndExit = true;
                        }
                    case '?':
                        throw std::invalid_argument("no such option");
                    case 0:
                        switch(option_index) {
                            case 3: {
                                    options.verifyOptions = VerifyOptions(optarg);
                                } break;
                            case 5: {
                                    if (optarg == NULL) {
                                        // no data, random seed
                                        options.seed = time(NULL);
                                    } else {
                                        std::stringstream ssSeed(optarg);
                                        ssSeed >> options.seed;
                                        if (ssSeed.fail()) {
                                            throw std::invalid_argument("--seed requires an positive integer argument");
                                        }
                                    }
                                } break;
                            case 6: {
                                // TODO better logic
                                options.colorMode = Logger::COLOR_ANSI;
                                } break;
                            default: {
                                    std::stringstream message;
                                    message << "0 default: " << (int)option_index;
                                    throw std::invalid_argument(message.str());
                                } break;
                        } break;
                    default: {
                            std::stringstream message;
                            message << "default: " << c;
                            throw std::invalid_argument(message.str());
                        }
                }
            }

                // other arguments, we expect exactly two decks
            if(optind+2 == argc) {
                options.attackDeck.setHash(argv[optind+0]);
                options.defenseDeck.setHash(argv[optind+1]);
            } else {
                throw std::invalid_argument("please specify exactly two decks to test");
            }
            return options;
        }

    } // namespace CLI
} // namespace EvaluateDecks
