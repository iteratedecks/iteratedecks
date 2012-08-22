#ifndef LOGGER_HPP_
    #define LOGGER_HPP_

    #include <string>
    #include "deck.hpp"

    class Logger {
        public:
            enum ColorMode {
                COLOR_NONE,
                COLOR_ANSI,
                COLOR_CSS
            };
            static unsigned int const LOG_NONE;
            static unsigned int const LOG_ABILITY;
            static unsigned int const LOG_ABILITY_FAILED;
            static unsigned int const LOG_ABILITY_FAILED_NOTARGET;
            static unsigned int const LOG_TURN;
            static unsigned int const LOG_ALL;
        private:
            unsigned int flags;
            CardDB const * const cDB;
            ColorMode colorMode;

            void write(std::string const & message);
        public:
            std::string abilityIdToString(AbilityId const & abilityId) const;
        public:
            std::string colorCard(PlayedCard const & playedCard) const;
            std::string colorTurn(std::string const & str) const;
        public:
            Logger(unsigned int const & flags, CardDB const * const & cDB);
            void setColorMode(ColorMode const & colorMode);

            void log(unsigned int const & flags, std::string const & message);
            bool isEnabled(unsigned int const & flags) const;
    };

    class SimulationLogger {
        private:
            Logger delegate;
        public:
            std::string colorTurn(std::string const & str) const;
        public:
            SimulationLogger(Logger const & delegate);
            void turnStart(unsigned int const & turn);
            void turnEnd(unsigned int const & turn);
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

            void abilityAttack(PlayedCard const & src, std::string const & message);
            void abilitySupport(EVENT_CONDITION const & eventCondition,PlayedCard const & src, AbilityId const & aid, PlayedCard const & target, EFFECT_ARGUMENT amount);

            void abilitySummon(EVENT_CONDITION const & eventCondition, PlayedCard const & src, PlayedCard const & summonedCard);
            void abilityFlurry(PlayedCard const & src, EFFECT_ARGUMENT const & amount);


            void abilityFailNoTarget(EVENT_CONDITION const & eventCondition, AbilityId const & aid, PlayedCard const & src,bool const & isMimiced, bool const & isChaosed,FactionId const & factionId,EFFECT_ARGUMENT const & effectArgument);
            void abilityFailDisease(EVENT_CONDITION const & eventCondition, AbilityId const & aid, PlayedCard const & src, PlayedCard const & target, bool const & isMimiced, FactionId const & factionId, EFFECT_ARGUMENT const & effectArgument);
    };

    #define LOG(logger,command) do { if((logger) != NULL) { (logger)->command; } } while(false)

#endif
