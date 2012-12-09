/**
 * Copyright 2012 Fabian "Praetorian" Kürten.
 *
 * This file is part of IterateDecks.
 *
 *  IterateDecks is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, only version 3 of the License.
 *
 *  IterateDecks is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with IterateDecks.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  In addition this file can be redistributed and/or modified under
 *  version 3 of the GNU General Public License or any later version
 *  of the License. (This does not extend to other files unless noted.)
 */

#ifndef CLI_PARSER_HPP_
    #define CLI_PARSER_HPP_

    #include "cliOptions.hpp"

    /**
     * Namespace for the application.
     */
    namespace EvaluateDecks {

        /**
         * Namespace for stuff related to the CLI.
         */
        namespace CLI {

            /**
             * @brief parses options provided on the commandline and returns them in an object
             *
             * The available options (and their semantic) are defined in the corresponding source file.
             *
             * @param argc the argument count, as provided in main
             * @param argv the arguments, as provided in main
             */
            CliOptions parseCliOptions(int const & argc, char * const argv[])  throw (std::invalid_argument, std::logic_error);

        } // namespace CLI
    } // namespace EvaluateDecks

#endif
