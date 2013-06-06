#include "ownedCardsParser.hpp"
#include <sstream>
#include "../CORE/exceptions.hpp"
#include "../CORE/assert.hpp"
#include <ios>
#include <iostream>

namespace IterateDecks {
    namespace Core {

        std::multiset<unsigned int>
        parseSimpleSeparatedString(std::istream & is
                                  ,std::string const & rowSeparator
                                  ,std::string const & columnSeparator
                                  )
        {
            std::istream::iostate const oldExceptionFlags = is.exceptions();
            //is.exceptions(std::istream::failbit | std::istream::badbit);            
            std::multiset<unsigned int> cards;
            int row = 0;

            try {
                while (!is.eof()) {
                    assertX(is.good());
                    
                    // we expect a card identifier enclosed in parentheses
                    char buffer[1];
                    is.read(buffer, 1);
                    if (buffer[0] == '\n') {
                        // strange newline near end of file which is not on the disk
                        //std::clog << "strange newline" << std::endl;
                        continue;
                    } else if (buffer[0] != '"') {
                        std::stringstream ssMessage;
                        ssMessage << "Expected an opening '\"' but got '";
                        ssMessage << buffer[0];
                        ssMessage << "' (that is ";
                        ssMessage << static_cast<int>(buffer[0]);
                        ssMessage << " in decimal)" ;
                        ssMessage << " in row " << row << ".";
                        throw InvalidUserInputError(ssMessage.str());
                    }
                    unsigned int cardId;
                    is >> cardId;
                    if (!is.good()) {
                        std::stringstream ssMessage;
                        ssMessage << "IO failure. Tried to read a card id last.";
                        ssMessage << " in row " << row << ".";
                        throw InvalidUserInputError(ssMessage.str());
                    }
                    is.read(buffer, 1);
                    if (buffer[0] != '"') {
                        std::stringstream ssMessage;
                        ssMessage << "Expected an closing '\"' but got '";
                        ssMessage << buffer[0];
                        ssMessage << "'";
                        ssMessage << " in row " << row << ".";
                        throw InvalidUserInputError(ssMessage.str());
                    }

                    // now we expect a column separator
                    for(size_t i = 0; i < columnSeparator.size(); i++) {
                        is.read(buffer, 1);
                        if(buffer[0] != columnSeparator[i]) {
                            std::stringstream ssMessage;
                            ssMessage << "Expected a row separator (";
                            ssMessage << columnSeparator;
                            ssMessage << ") but got a ";
                            ssMessage << buffer[0];
                            ssMessage << " at position " << i;
                            ssMessage << " in row " << row << ".";
                            throw InvalidUserInputError(ssMessage.str());
                        }
                    }

                    // now the number of times the card appears
                    unsigned int count;
                    is >> count;
                    if (is.bad() || is.fail()) {
                        throw InvalidUserInputError("IO failure. Tried to read a card count last.");
                    }

                    for(size_t i = 0; i < count; i++) {
                        cards.insert(cardId);
                    }
                    //std::clog << "got " << cardId << " * " << count << " in row " << row << std::endl;

                    // at this point we may stop
                    if (is.eof()) {
                        break;
                    }

                    // now a row separator
                    for(size_t i = 0; i < rowSeparator.size(); i++) {
                        is.read(buffer, 1);
                        if(buffer[0] != rowSeparator[i]) {
                            std::stringstream ssMessage;
                            ssMessage << "Expected a row separator (";
                            ssMessage << columnSeparator;
                            ssMessage << ") but got a ";
                            ssMessage << buffer[0];
                            ssMessage << " at position " << i;
                            ssMessage << " in row " << row << ".";                        
                            throw InvalidUserInputError(ssMessage.str());
                        }
                    }

                    if (is.bad() || is.fail()) {
                        throw InvalidUserInputError("IO failure. Tried to read a row separator last.");
                    }

                    //std::clog << "finished row " << row << std::endl;
                    row++;                
                }
            } catch (std::istream::failure & e) {
                std::stringstream ssMessage;
                ssMessage << "Got am ios_base::failure with error code ";
                // gcc is broken here :(
                #if (__GNUC__ == 4) and (__GNUC_MINOR__ <= 7)
                    ssMessage << "(broken gcc does not support new ios_base::failure)";
                #else
                    ssMessage << e.code();
                #endif
                ssMessage << std::endl;
                if (!is.good()) {
                    ssMessage << "with flags ";
                    if (is.bad()) {
                        ssMessage << "bad ";
                    }
                    if (is.fail()) {
                        ssMessage << "fail ";
                    }
                    if (is.eof()) {
                        ssMessage << "eof ";
                    }
                    ssMessage << std::endl;
                }
                ssMessage << e.what();
                throw Exception(ssMessage.str());
            }
            is.exceptions(oldExceptionFlags);
            return cards;            
        }
                                  
        
    }
}
