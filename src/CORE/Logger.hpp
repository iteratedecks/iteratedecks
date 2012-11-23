#ifndef LOGGER_HPP_
    #define LOGGER_HPP_

    #include <string>
    #include "deck.hpp"
    #include "playedCard.hpp"

    namespace IterateDecks { namespace Core {

    class Logger {
        public:
            enum ColorMode {
                COLOR_NONE,
                COLOR_ANSI,
                COLOR_CSS
            };
            static unsigned long const LOG_NONE;
            static unsigned long const LOG_ABILITY;
            static unsigned long const LOG_ABILITY_FAILED;
            static unsigned long const LOG_ABILITY_FAILED_NOTARGET;
            static unsigned long const LOG_ATTACK;
            static unsigned long const LOG_ATTACK_BEGIN_END;
            static unsigned long const LOG_DEFENSIVE_REFRESH;
            static unsigned long const LOG_TURN;
            static unsigned long const LOG_SIMULATION;
            static unsigned long const LOG_ALL;
        private:
            unsigned long flags;
            CardDB const * const cDB;
            ColorMode colorMode;

            void write(std::string const & message);
        public:
            std::string abilityIdToString(AbilityId const & abilityId) const;
        public:
            std::string colorCard(PlayedCard const & playedCard) const;
            std::string colorTurn(std::string const & str) const;
            std::string colorSimulation(std::string const & str) const;
        public:
            Logger(unsigned long const & flags, CardDB const * const & cDB);
            void setColorMode(ColorMode const & colorMode);

            void log(unsigned long const & flags, std::string const & message);
            bool isEnabled(unsigned long const & flags) const;
    };

    class SimulationLogger {
        private:
            Logger delegate;
        public:
            std::string colorTurn(std::string const & str) const;
            std::string colorSimulation(std::string const & str) const;
        public:
            SimulationLogger(Logger const & delegate);
            void turnStart(unsigned int const & turn);
            void turnEnd(unsigned int const & turn);
            void simulationStart(unsigned int const & simulation);
            void simulationEnd(unsigned int const & simulation);
    };

    class DeckLogger {
        public:
            enum DeckType {
                ATTACK,
                DEFENSE
            };
        private:
            Logger delegate;
            DeckType deckType;
        protected:
            std::string getDeckStr() const;
        public:
            std::string colorCard(PlayedCard const & playedCard) const;
        public:
            DeckLogger(DeckType const & deckType, Logger const & delegate);
        
            void ability(EVENT_CONDITION const & eventCondition,PlayedCard const & src, std::string const & message);
            void attack(PlayedCard const & src, std::string const & message);

            void abilitySupport(EVENT_CONDITION const & eventCondition,PlayedCard const & src, AbilityId const & aid, PlayedCard const & target, EFFECT_ARGUMENT amount);
            void abilityOffensive(EVENT_CONDITION const & eventCondition,PlayedCard const & src, AbilityId const & aid, PlayedCard const & target, EFFECT_ARGUMENT amount, bool evaded = false);

            void abilitySummon(EVENT_CONDITION const & eventCondition, PlayedCard const & src, PlayedCard const & summonedCard);
            void abilityFlurry(PlayedCard const & src, EFFECT_ARGUMENT const & amount);
            void abilityTribute(EVENT_CONDITION const & eventCondition, PlayedCard const & tributer, PlayedCard const & tributee,AbilityId const & abilityId,EFFECT_ARGUMENT const & effectArgument);


            void abilityFailNoTarget(EVENT_CONDITION const & eventCondition, AbilityId const & aid, PlayedCard const & src,bool const & isMimiced, bool const & isChaosed,FactionId const & factionId,EFFECT_ARGUMENT const & effectArgument);
            void abilityFailDisease(EVENT_CONDITION const & eventCondition, AbilityId const & aid, PlayedCard const & src, PlayedCard const & target, bool const & isMimiced, FactionId const & factionId, EFFECT_ARGUMENT const & effectArgument);
            void abilityFailNoProc(EVENT_CONDITION const & eventCondition, PlayedCard const & src, AbilityId const & aid, PlayedCard const & target);


            void attackBegin(PlayedCard const & attacker);
            void attackSwipe(PlayedCard const & attacker);
            void attackTarget(PlayedCard const & attacker, PlayedCard const & target);
            void attackBonus(PlayedCard const & attacker, PlayedCard const & target, AbilityId abilityId, EFFECT_ARGUMENT amount);
            void attackHit(PlayedCard const & attacker, PlayedCard const & victim, unsigned int const & damage);
            void attackEnd(PlayedCard const & attacker);

            void defensiveAbility(PlayedCard const & victim, PlayedCard const & attacker, AbilityId abilityId, EFFECT_ARGUMENT amount);
            void defensiveRefresh(EVENT_CONDITION const & eventCondition, PlayedCard const & unit, EFFECT_ARGUMENT amount);
    };

    #define LOG(logger,command) do { if((logger) != NULL) { (logger)->command; } } while(false)

    }}

#endif
