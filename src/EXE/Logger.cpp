#include "Logger.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>

unsigned int const Logger::LOG_NONE(0);
unsigned int const Logger::LOG_ABILITY(1);
unsigned int const Logger::LOG_TURN(2);
unsigned int const Logger::LOG_ALL(3);

std::string eventConditionToString(EVENT_CONDITION const & eventCondition)
{
    if(eventCondition == EVENT_EMPTY) {
        return std::string();
    } else if(eventCondition == EVENT_ATTACKED) {
        return "On Attack ";
    } else if(eventCondition == EVENT_DIED) {
        return "On Death ";
    } else if(eventCondition == EVENT_PLAYED) {
        return "On Play ";
    } else {
        throw std::invalid_argument("Unknown EVENT_CONDITION");
    }
}

Logger::Logger(unsigned int const & flags)
: flags(flags)
{
}

void Logger::log(unsigned int const & flags
           ,std::string const & message
           )
{
    if(this->isEnabled(flags)) {
        this->write(message);
    }
}

void Logger::write(std::string const & message)
{
    std::cout.flush();
    std::clog << message << std::endl;
}

bool Logger::isEnabled(unsigned int const & flags) const
{
    return (this->flags & flags) > 0;
}


SimulationLogger::SimulationLogger(Logger const & delegate)
: delegate(delegate)
{
}

void SimulationLogger::turnStart(unsigned int const & turn)
{
    if (this->delegate.isEnabled(Logger::LOG_TURN)) {
        std::stringstream ssMessage;
        ssMessage << "Turn " << turn << " starts";
        std::string message(ssMessage.str());
        this->delegate.log(Logger::LOG_TURN,message);
    }
}

void SimulationLogger::turnEnd(unsigned int const & turn)
{
    if (this->delegate.isEnabled(Logger::LOG_TURN)) {
        std::stringstream ssMessage;
        ssMessage << "Turn " << turn << " ends";
        std::string message(ssMessage.str());
        this->delegate.log(Logger::LOG_TURN,message);
    }
}


DeckLogger::DeckLogger(DeckType const & deckType, Logger const & delegate)
: delegate(delegate)
, deckType(deckType)
{
}

std::string DeckLogger::getDeckStr() const
{
    switch(this->deckType) {
        case ATTACK:
            return "Off Deck ";
        case DEFENSE:
            return "Def Deck ";
        default:
            throw std::invalid_argument("Unknown DeckType");
    }
}

void DeckLogger::ability(EVENT_CONDITION const & eventCondition
                        ,PlayedCard const & src
                        ,std::string const & message
                        )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY)) {
        std::string eventConditionStr = eventConditionToString(eventCondition);
        std::string deckStr = this->getDeckStr();
        this->delegate.log(Logger::LOG_ABILITY,deckStr + eventConditionStr + src.toString() + message);
    }
}

void DeckLogger::abilitySummon(EVENT_CONDITION const & eventCondition
                              ,PlayedCard const & src
                              ,PlayedCard const & summonedCard
                              )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY)) {
        std::string summonMessage("summons " + summonedCard.toString());
        this->ability(eventCondition,src,summonMessage);
    }
}
