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

#ifdef _WIN32
    #define __windows__
#endif
#ifdef _WIN64
    #define __windows__
#endif

#if defined(__windows__)
	#include "../../EvaluateDecks/getopt_mb_uni_vc10/getopt_mb_uni_vc10/getopt.h"
#else
	#include <getopt.h>
#endif

#include <sstream>
#include <ctime>

namespace EvaluateDecks {
    namespace CLI {

        CliOptions parseCliOptions(int const & argc, char * const argv[]) throw (std::invalid_argument, std::logic_error)
        {
            // get option table
			option long_options[numberOfOptions];
            std::string shortOptions;
            for(unsigned int i = 0; i < numberOfOptions; i++) {
                long_options[i] = options[i].getOptPart;
                if (long_options[i].flag == NULL && long_options[i].val != 0) {
                    shortOptions += long_options[i].val;
                    if(long_options[i].has_arg == required_argument) {
                        shortOptions += ":";
                    } else if (long_options[i].has_arg == optional_argument) {
                        shortOptions += "::";
                    }
                }
            }
            char const * const short_options(shortOptions.c_str());

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
                        } break;
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
            if (options.printHelpAndExit) {
                // no more arguments expected
            } else if(optind+2 == argc) {
                // other arguments, we expect exactly two decks
                options.attackDeck.setHash(argv[optind+0]);
                options.defenseDeck.setHash(argv[optind+1]);
            } else {
                throw std::invalid_argument("please specify exactly two decks to test");
            }
            return options;
        }

    } // namespace CLI
} // namespace EvaluateDecks
