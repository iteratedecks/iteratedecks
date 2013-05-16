#include "commands.hpp"
#include "../VERSION.h"
#include <iostream>
#include <boost/program_options.hpp>

namespace IterateDecks {
    namespace CLI3 {

        Command::~Command() {}

        int VersionCommand::execute()
        {
            std::cout << ITERATEDECKS_VERSION;
            if (ITERATEDECKS_DIRTY_HEAD) {
                std::cout << " locally modified!";
            }
            std::cout << std::endl;
            return 0;
        }

        HelpCommand::HelpCommand(boost::program_options::options_description const & desc)
        : desc(desc)
        {
        }

        int HelpCommand::execute()
        {
            std::cout << this->desc << std::endl;
            return 0;
        }

    }
}
