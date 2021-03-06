#include "commands.hpp"
#include "../VERSION.h"
#include <iostream>
#include <boost/program_options.hpp>
#include "../CORE/iterateDecksCore.hpp"
#include <iomanip>
#include "../CORE/assert.hpp"

using namespace IterateDecks::Core;
namespace IterateDecks {
    namespace CLI3 {

        Command::Command()
        : aborted(false)
        {}

        Command::~Command()
        {}

        void
        Command::abort()
        {
            this->aborted = true;
        }


        int VersionCommand::execute(Core::CardDB const & cardDB)
        {
            //human readable
            std::cout << "human readable version description:" << std::endl;
            std::cout << "\t" << ITERATEDECKS_VERSION_DESCRIBE;
            if (ITERATEDECKS_DIRTY_HEAD) {
                std::cout << "+";
                std::cout << ITERATEDECKS_DIRTY_HASH;
            }
            std::cout << std::endl;

            // commit-ish
            std::cout << "iteratedecks-cli3 version hash:" << std::endl;
            std::cout << '\t' << ITERATEDECKS_VERSION;
            if (ITERATEDECKS_DIRTY_HEAD) {
                std::cout << "+";
                std::cout << ITERATEDECKS_DIRTY_HASH;
            }
            std::cout << std::endl;

            // tags
            if (!ITERATEDECKS_DIRTY_HEAD) {
                std::cout << "Tags pointing at this version: " << std::endl;
                std::cout << '\t' << ITERATEDECKS_VERSION_TAGS << std::endl;
            }

            // TODO Obviously if we want to allow exchangable core we would need a better way here.
            SimulatorCore::Ptr core = SimulatorCore::Ptr( new IterateDecksCore(cardDB) );

            std::cout << "Simulator core name: "    << std::endl;
            std::cout << '\t' << core->getCoreName()        << std::endl;
            std::cout << "Simulator core version: " << std::endl;
            std::cout << '\t' << core->getCoreVersion()     << std::endl;

            std::cout << "XML data version: "       << std::endl;
            std::map<std::string,std::string> xmlHashes = core->getXMLVersions();
            assertGT(xmlHashes.size(),0u);
            for(std::map<std::string,std::string>::const_iterator iter = xmlHashes.begin()
               ;iter != xmlHashes.end()
               ;iter++
               )
            {
                std::cout << '\t';
                std::cout << std::setw(20) << std::left << iter->first << " ";
                std::cout << iter->second << std::endl;
            }
            return 0;
        }

        int CoreVersionCommand::execute(Core::CardDB const & cardDB)
        {
            // TODO Obviously if we want to allow exchangable core we would need a better way here.
            SimulatorCore::Ptr core = SimulatorCore::Ptr( new IterateDecksCore(cardDB) );

            std::cout << core->getCoreVersionHumanReadable() << std::endl;
            return 0;
        }

        HelpCommand::HelpCommand(boost::program_options::options_description const & desc)
        : desc(desc)
        {
        }

        int HelpCommand::execute(Core::CardDB const & cardDB)
        {
            std::cout << this->desc << std::endl;
            return 0;
        }

    }
}
