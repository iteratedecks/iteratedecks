#include "optionParser.hpp"
#include "commands.hpp"
#include "runCommand.hpp"

#include "../CORE/exceptions.hpp"
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include "../CORE/autoDeckTemplate.hpp"
#include "simpleOrderedDeckTemplate.hpp"

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
                    ("surge"
                    ,"simulate surge (defender goes first)"
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
                command->task.surge = vm.count("surge") > 0;
                return command;
             } catch (boost::program_options::required_option &e) {
                 std::stringstream ssMessage;
                 ssMessage << "Error parsing the arguments:" << std::endl;
                 ssMessage << e.what() << std::endl;
                 throw InvalidUserInputError(ssMessage.str());
             }
        }

        bool splitOnceAfterChar(char const delimiter
                              ,std::string const & subject
                              ,std::string & first
                              ,std::string & remainder
                              )
        {
            size_t const position = subject.find(delimiter);
            if (position != std::string::npos) {
                first = subject.substr(0,position);
                remainder = subject.substr(position+1);
                return true;
            } else {
                return false;
            }
        }

        DeckTemplate::Ptr parseDeckFromIds(std::string const & data, bool ordered = false) {
            // we expect ids (numbers) seperated by commata
            std::list<unsigned int> ids;
            std::string remainder = data;
            while (true) {
                std::string number;
                bool splitSuccess = splitOnceAfterChar(',', remainder, number, remainder);                
                if (splitSuccess) {
                    unsigned int id = boost::lexical_cast<unsigned int>(number);
                    ids.push_back(id);
                } else {
                    // last one
                    unsigned int id = boost::lexical_cast<unsigned int>(remainder);
                    ids.push_back(id);
                    break;
                }
            }
            if (!ordered) {
                return DeckTemplate::Ptr( new AutoDeckTemplate(ids) );
            } else {
                return DeckTemplate::Ptr( new SimpleOrderedDeckTemplate(ids) );
            }
        }
        
        DeckTemplate::Ptr parseDeck(std::string const & deckDescription
                                   )
        {
            // valid deck descriptions start with a string part describing what type of deck this is
            // the format is: IDENTIFIER:DATA        
            std::string identifier, data;
            bool splitSuccess = splitOnceAfterChar(':', deckDescription, identifier, data);
            if (!splitSuccess) {
                throw InvalidUserInputError("Deck description '" + deckDescription + "' has invalid form. Format is 'IDENTIFIER:DATA'");
            }
            //std::clog << "Identifier is " << identifier << std::endl;
            //std::clog << "data is " << data << std::endl;
            if (identifier.compare("IDS") == 0) {
                // IDS, thats good
                return parseDeckFromIds(data);
            } else if (identifier.compare("IDS") == 0) {
                // ORDERED_IDS, thats ok
                return parseDeckFromIds(data, true);
            } else {
                throw InvalidUserInputError("Identifier '" + identifier + "' not supported, try one of {'IDS', 'ORDERED_IDS'}");
            }
        }


    }
}
