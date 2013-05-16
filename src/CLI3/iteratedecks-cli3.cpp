#include "../CORE/iterateDecksCore.hpp"
#include <iostream>
#include "optionParser.hpp"

using namespace IterateDecks::Core;
using namespace IterateDecks::CLI3;

/**
 * Reads cli arguments, parses them, then simulates
 */
int main(int const argc, char * const * const argv)
{
    try {
        Command::Ptr command = parseArguments(argc, argv);

        command->execute();
        
    } catch(Exception const & e) {
        std::cerr << "Exception:" << std::endl;
        std::cerr << e.what() << std::endl;
        e.printStacktrace(std::cerr);
        return -1;    
    }
}
