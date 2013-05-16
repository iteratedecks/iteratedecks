#include "optionParser.hpp"
#include "commands.hpp"
#include "runCommand.hpp"

#include "../CORE/exceptions.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace IterateDecks {
    namespace CLI3 {

        Command::Ptr parseArguments(int argc
                                   ,char const * const * argv
                                   )
        {
            try {
                unsigned int numberOfIterations;
                std::string attacker, defender;
                
                po::options_description desc("Allowed options");
                desc.add_options()
                    ("help,h", "produce help message")
                    ("version,V", "version information")
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
                ;

                po::variables_map vm;
                po::store(po::parse_command_line(argc, argv, desc), vm);
                po::notify(vm);

                if (vm.count("help")) {
                    return Command::Ptr(new HelpCommand(desc));
                } else if (vm.count("version")) {
                    return Command::Ptr(new VersionCommand());
                }

                RunCommand::Ptr command = RunCommand::Ptr(new RunCommand());
                command->task.minimalNumberOfGames = numberOfIterations;
                command->task.attacker = parseDeck(vm["attacker"].as<std::string>());
                command->task.defender = parseDeck(vm["defender"].as<std::string>());
                return command;
             } catch (boost::program_options::required_option &e) {
                 std::stringstream ssMessage;
                 ssMessage << "Error parsing the arguments:" << std::endl;
                 ssMessage << e.what() << std::endl;
                 throw InvalidUserInputError(ssMessage.str());
             }
        }

        DeckTemplate::Ptr parseDeck(std::string const & deckDescription
                                   )
        {
            throw Exception("Not implemented!");
        }


    }
}
