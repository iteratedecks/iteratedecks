#include "optionParser.hpp"
#include "commands.hpp"
#include "runCommand.hpp"

#include "../CORE/exceptions.hpp"
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include "../CORE/autoDeckTemplate.hpp"
#include "simpleOrderedDeckTemplate.hpp"
#include "../CORE/simpleTypes.hpp"
#include "../CORE/assert.hpp"

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
                unsigned int battleGroundId;

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
                    ("battleground-id"
                    ,po::value<unsigned int>(&battleGroundId)->default_value(static_cast<unsigned int>(BattleGroundEffect::normal))
                    ,"set the battleground to use"
                    )
                ;

                po::variables_map vm;
                po::store(po::parse_command_line(argc, argv, desc), vm);

                if (vm.count("help")) {
                    return Command::Ptr(new HelpCommand(desc));
                } else if (vm.count("version")) {
                    return Command::Ptr(new VersionCommand());
                }

                po::notify(vm);

                RunCommand::Ptr command = RunCommand::Ptr(new RunCommand());
                command->task.minimalNumberOfGames = numberOfIterations;
                command->task.attacker = parseDeck(vm["attacker"].as<std::string>());
                command->task.defender = parseDeck(vm["defender"].as<std::string>());
                command->task.surge = vm.count("surge") > 0;
                command->task.battleGround = static_cast<BattleGroundEffect>(battleGroundId);
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

        std::list<unsigned int> base64RLEMinusEncodingToIdList(std::string const & hash)
        {
            std::list<unsigned int> list;
            size_t len = hash.size();

            //std::clog << "parsing hash " << hash << std::endl;

            unsigned int lastid;
            for (UCHAR i = 0; i < len; i+=2) {
                //std::clog << "current character is '" << hash[i] << "'" << std::endl;
                if (hash[i] == '.') break; // delimeter
                if (isspace(hash[i])) {
                    assertX(false);
                }
                unsigned int tid = 0;
                bool isCardOver4000 = false;
                if(hash[i] == '-') {
                    i++;
                    isCardOver4000 = true;
                    tid = 4000;
                }
                assertX(i + 1u < len); // make sure we have a full hash
                //std::clog << "reading characters '" << hash[i] << hash[i+1] << "' ";
                unsigned short cardHash = (hash[i] << 8) + hash[i + 1];
                tid += base64ToId(cardHash);
                //std::clog << "tid is " << tid << std::endl;
                if (i==0) {
                    // first card is commander
                    assertX(tid < CARD_MAX_ID);
                    assertX((tid >= 1000) && (tid < 2000)); // commander Id boundaries
                    //std::clog << "adding commander " << tid << std::endl;
                    list.push_back(tid);
                } else {
                    // later cards are not commander
                    assertX(i>0 || (tid < CARD_MAX_ID)); // commander card can't be encoded with RLE

                    if (tid < 4000 || isCardOver4000) {
                        // this is a (non commander) card
                        list.push_back(tid);
                        //std::clog << "adding card " << tid << " the first time " << std::endl;
                        lastid = tid;
                    } else {
                        // this is a RLE
                        for (unsigned int k = 4000+1; k < tid; k++) {
                            // decode RLE, +1 because we already added one card
                            list.push_back(lastid);
                            //std::clog << "adding card " << lastid << " again " << std::endl;
                        } // for RLE
                    }
                }
            } // for

            //for(std::list<unsigned int>::const_iterator i = list.begin(); i != list.end(); i++) {
            //    std::clog << *i << " ";
            //}
            //std::clog << std::endl;
            return list;
        }

        DeckTemplate::Ptr parseDeckFromIds(std::string const & data, bool ordered = false) {
            // we expect ids (numbers) seperated by commata
            std::list<unsigned int> ids;
            std::string remainder = data;
            while (true) {
                std::string number;
                bool splitSuccess = splitOnceAfterChar(',', remainder, number, remainder);
                if (!splitSuccess) {
                    // last one
                    number = remainder;
                }
                try {
                    unsigned int id = boost::lexical_cast<unsigned int>(number);
                    ids.push_back(id);
                } catch (boost::bad_lexical_cast &e)    {
                    throw InvalidUserInputError("Bad input format, use positive numbers separated by commata.");
                }
                if (!splitSuccess) {
                    break;
                }
            } // while
            if (!ordered) {
                return DeckTemplate::Ptr( new AutoDeckTemplate(ids) );
            } else {
                return DeckTemplate::Ptr( new SimpleOrderedDeckTemplate(ids) );
            }
        }

        DeckTemplate::Ptr
        parseDeckFromStrangeBase64RLEMinusEncoding(std::string const & base64RLEMinusEncoding
                                                  ,bool ordered = false
                                                  )
        {
            std::list<unsigned int> ids = base64RLEMinusEncodingToIdList(base64RLEMinusEncoding);
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
            } else if (identifier.compare("ORDERED_IDS") == 0
                    || identifier.compare("IDS_ORDERED") == 0 ) {
                // ORDERED_IDS, thats ok
                return parseDeckFromIds(data, true);
            } else if (identifier.compare("BASE64RLEMINUS") == 0) {
                // freaky base64 encoding
                return parseDeckFromStrangeBase64RLEMinusEncoding(data);
            } else if (identifier.compare("ORDERED_BASE64RLEMINUS") == 0
                    || identifier.compare("BASE64RLEMINUS_ORDERED") == 0) {
                // freaky base64 encoding
                return parseDeckFromStrangeBase64RLEMinusEncoding(data, true);
            } else if (identifier.compare("MISSIONID") == 0) {
                throw Exception("Sorry, not implemented yet!");
            } else if (identifier.compare("RAIDID") == 0) {
                throw Exception("Sorry, not implemented yet!");
            } else if (identifier.compare("QUESTID") == 0) {
                throw Exception("Sorry, not implemented yet!");
            } else {
                std::stringstream ssMessage;
                ssMessage << "Identifier '" << identifier << "' not supported." << std::endl;
                ssMessage << "Try one of the following:" << std::endl;
                ssMessage << "\t'IDS', 'ORDERED_IDS', 'IDS_ORDERED' for id based input" << std::endl;
                ssMessage << "\t'BASE64RLEMINUS', 'ORDERED_BASE64RLEMINUS', 'BASE64RLEMINUS_ORDERED', for some strange base64 encoding with an extra minus" << std::endl;
                ssMessage << "\t'MISSIONID', for missions, use the id, not the name" << std::endl;
                ssMessage << "\t'RAIDID', for raids, use the id, not the name" << std::endl;
                ssMessage << "\t'QUESTID', for quest steps, use the id, not the name" << std::endl;
                throw InvalidUserInputError(ssMessage.str());
            }
        }


    }
}
