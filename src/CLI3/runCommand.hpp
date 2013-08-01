#ifndef ITERATEDECKS_CLI3_RUNCOMMAND_HPP
    #define ITERATEDECKS_CLI3_RUNCOMMAND_HPP

    #include "../CORE/iterateDecksCore.hpp"
    #include "../OPT/optimizer.hpp"
    #include "commands.hpp"
    #include <memory>

    namespace IterateDecks {
        namespace CLI3 {

            class RunCommand : public Command {
                public:
                    typedef std::shared_ptr<RunCommand> Ptr;
                public:
                    IterateDecks::Core::SimulationTaskClass task;
                    bool optimizeAttacker;
                    bool optimizeDefender;
                    IterateDecks::Opt::OptimizationTarget optimizationTarget;
                    int verbosity;
                    IterateDecks::Core::Logger* logger;
                private:
                    IterateDecks::Core::SimulatorCore::Ptr simulator;
                    IterateDecks::Opt::Optimizer::Ptr optimizer;
                public:
                    RunCommand(int verbosity, bool noCacheRead, unsigned int extraCards, Core::CardDB const & cardDB);
                    ~RunCommand();

                    int execute(Core::CardDB const & cardDB);
                    void abort();
            };
        }
    }

#endif
