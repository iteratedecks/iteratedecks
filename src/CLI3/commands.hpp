#ifndef ITERATEDECKS_CLI3_COMMANDS_HPP
    #define ITERATEDECKS_CLI3_COMMANDS_HPP

    #include <memory>
    #include <boost/program_options.hpp>

    #include "../CORE/cardDB.hpp"

    namespace IterateDecks {
        namespace CLI3 {

            class Command {
                private:
                    bool aborted;
                protected:
                    bool isAborted();

                public:
                    typedef std::shared_ptr<Command> Ptr;
                public:
                    Command();
                    virtual ~Command();
                    virtual int execute(Core::CardDB const & cardDB) = 0;
                    virtual void abort();
            };

            class VersionCommand : public Command {
                public:
                    int execute(Core::CardDB const & cardDB);
            };

            class CoreVersionCommand : public Command {
                public:
                    int execute(Core::CardDB const & cardDB);
            };

            class HelpCommand : public Command {
                private:
                    boost::program_options::options_description desc;
                public:
                    HelpCommand(boost::program_options::options_description const & desc);
                    int execute(Core::CardDB const & cardDB);
            };

        }
    }
#endif
