#include "optionParser.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <cctype>
#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <new>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "../CORE/achievementOptions.hpp"
#include "../CORE/activeDeck.forward.hpp"
#include "../CORE/assert.hpp"
#include "../CORE/autoDeckTemplate.hpp"
#include "../CORE/cardDB.hpp"
#include "../CORE/constants.hpp"
#include "../CORE/deckTemplate.hpp"
#include "../CORE/exceptions.hpp"
#include "../CORE/iterateDecksCore.hpp"
#include "../CORE/questDeck.hpp"
#include "../CORE/raidDeck.hpp"
#include "../CORE/simpleTypes.hpp"
#include "commands.hpp"
#include "missionIdDeckTemplate.hpp"
#include "runCommand.hpp"
#include "simpleOrderedDeckTemplate.hpp"
#include "../CORE/multiDeckTemplate.hpp"

#include "deckParser.hpp"

namespace po = boost::program_options;
using namespace IterateDecks::Core;
namespace IterateDecks {
    namespace CLI3 {

        Command::Ptr parseArguments(Core::CardDB const & cardDB
                                   ,int argc
                                   ,char const * const * argv
                                   )
        {
            try {
                unsigned int numberOfIterations;
                std::string attacker, defender;
                unsigned int battleGroundId;
                int achievementId;
                bool allowInvalidDecks;

                po::options_description desc("Allowed options");
                desc.add_options()
                    ("help,h", "produce help message")
                    ("version,V", "version information")
                    ("core-version", "version of the core")
                    ("verbose,v", "increase verbosity")
                    ("number-of-iterations,n"
                    ,po::value<unsigned int>(&numberOfIterations)->default_value(1000)
                    ,"specify the total number of iterations"
                    )
                    ("attacker"
                    ,po::value<std::string>(&attacker)->required()
                    ,"the attacker"
                    )
                    ("defender"
                    ,po::value<std::string>(&attacker)->required()
                    ,"the defender"
                    )
                    ("surge"
                    ,"simulate surge (defender goes first)"
                    )
                    ("battleground-id"
                    ,po::value<unsigned int>(&battleGroundId)->default_value(static_cast<unsigned int>(IterateDecks::Core::BattleGroundEffect::normal))
                    ,"set the battleground to use"
                    )
                    ("achievement-id"
                    ,po::value<int>(&achievementId)->default_value(-1)
                    ,"id of the achievement to check"
                    )
                    ("optimize-attacker"
                    ,"optimize the attacker's deck"
                    )
                    ("optimize-defender"
                    ,"optimize the defender's deck"
                    )
                    ("no-cache-read"
                    ,"do not read from the cache"
                    )
                    ("mutator-allow-extra"
                    ,po::value<unsigned int>()->default_value(0)
                    ,"allow up to this many cards not already owned"
                    )
                    ("allow-invalid-decks"
                    ,"allows invalid decks for the simulator. These are usually pointless but allow special test cases."
                    )
                ;

                po::variables_map vm;
                po::store(po::parse_command_line(argc, argv, desc), vm);

                if (vm.count("help")) {
                    return Command::Ptr(new HelpCommand(desc));
                } else if (vm.count("core-version")) {
                    return Command::Ptr(new CoreVersionCommand());
                } else if (vm.count("version")) {
                    return Command::Ptr(new VersionCommand());
                }

                po::notify(vm);

                RunCommand::Ptr command = RunCommand::Ptr(
                    new RunCommand(vm.count("verbose"), vm.count("no-cache-read") > 0, vm["mutator-allow-extra"].as<unsigned int>(), cardDB)
                );
                //std::clog << "running with " << numberOfIterations << " iterations" << std::endl;
                allowInvalidDecks = vm.count("allow-invalid-decks") > 0;
                command->task.minimalNumberOfGames = numberOfIterations;
                command->task.attacker = parseDeck(vm["attacker"].as<std::string>(), cardDB);
                command->task.attacker->allowInvalid = allowInvalidDecks;
                command->task.defender = parseDeck(vm["defender"].as<std::string>(), cardDB);
                command->task.defender->allowInvalid = allowInvalidDecks;
                command->task.surge = vm.count("surge") > 0;
                command->task.battleGround = static_cast<BattleGroundEffect>(battleGroundId);
                if (vm.count("achievement-id") > 0) {
                    command->task.achievementOptions.enableCheck(achievementId);
                }
                bool optimizeAttacker = vm.count("optimize-attacker") > 0;
                bool optimizeDefender = vm.count("optimize-defender") > 0;
                if(optimizeAttacker && optimizeDefender) {
                    throw InvalidUserInputError("Can not optimize both attacker and defender!");
                }
                command->optimizeAttacker = optimizeAttacker;
                command->optimizeDefender = optimizeDefender;
                return command;
             } catch (boost::program_options::required_option &e) {
                 std::stringstream ssMessage;
                 ssMessage << "Error parsing the arguments:" << std::endl;
                 ssMessage << e.what() << std::endl;
                 throw InvalidUserInputError(ssMessage.str());
             }
        }

        DeckTemplate::Ptr parseDeck(std::string const & deckDescription
                                   ,Core::CardDB const & cardDB
                                   )
        {
            //std::clog << "trying to parse deck with data " << deckDescription << std::endl;
            std::stringstream ssDescription(deckDescription);
            return parseDeckFromStream(ssDescription, cardDB);
        }


    }
}
