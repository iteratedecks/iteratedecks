#ifndef LOGGER_HPP_
    #define LOGGER_HPP_

    #include "deck.hpp"

    class Logger {
        private:
            unsigned int flags;
            void write(std::string message);
        public:
            void log(unsigned int const flags, std::string message);
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
        public:
            DeckLogger(DeckType const & deckType, Logger const & delegate);
        
            void ability(EVENT_CONDITION const & eventCondition,PlayedCard const & src, unsigned int abilityId, std::string const & message);
            void abilitySummon(EVENT_CONDITION const & eventCondition, PlayedCard const & src, Card const * const & summonedCard);
    };


#endif
