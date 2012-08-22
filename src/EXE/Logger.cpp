#include "Logger.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>

static int logInit=0;
unsigned int const Logger::LOG_NONE                     (0);
unsigned int const Logger::LOG_ABILITY                  (1<<logInit++);
unsigned int const Logger::LOG_ABILITY_FAILED           (1<<logInit++);
unsigned int const Logger::LOG_ABILITY_FAILED_NOTARGET  (1<<logInit++);
unsigned int const Logger::LOG_TURN                     (1<<logInit++);
unsigned int const Logger::LOG_ALL                      ((1<<logInit)-1);



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

std::string factionIdToString(FactionId const & factionId)
{
    if(factionId > 0) {
        char const * const factionName = FACTIONS[factionId];
        return std::string(factionName);
    } else {
        return std::string();
    }
}

Logger::Logger(unsigned int const & flags, CardDB const * const & cDB)
: flags(flags)
, cDB(cDB)
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
        this->delegate.log(Logger::LOG_ABILITY,deckStr + eventConditionStr + src.toString() + " " + message);
    }
}

void DeckLogger::abilityOffensive(PlayedCard const & src
                                 ,std::string const & message
                                 )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY)) {
        std::string deckStr = this->getDeckStr();
        this->delegate.log(Logger::LOG_ABILITY,deckStr + src.toString() + " " + message);
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

void DeckLogger::abilityFlurry(PlayedCard const & src
                              ,EFFECT_ARGUMENT const & amount
                              )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY)) {
        std::stringstream ssFlurryMessage;
        ssFlurryMessage << "flurries ";
        ssFlurryMessage << amount;
        std::string flurryMessage(ssFlurryMessage.str());
        this->abilityOffensive(src,flurryMessage);
    }
}

void DeckLogger::abilitySupport(EVENT_CONDITION const & eventCondition
                               ,PlayedCard const & src
                               ,AbilityId const & aid
                               ,PlayedCard const & target
                               ,EFFECT_ARGUMENT amount
                               )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY)) {
        std::stringstream ssMessage;
        ssMessage << this->delegate.abilityIdToString(aid) << " ";
        ssMessage << (unsigned int)amount << " ";
        ssMessage << target.toString();
        this->ability(eventCondition,src,ssMessage.str());
    }
}


void DeckLogger::abilityFailNoTarget(EVENT_CONDITION const & eventCondition
                                    ,AbilityId const & aid
                                    ,PlayedCard const & src
                                    ,bool const & isMimiced
                                    ,bool const & isChaosed
                                    ,FactionId const & factionId
                                    ,EFFECT_ARGUMENT const & effectArgument
                                    )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY_FAILED_NOTARGET)) {
        std::stringstream ssFailMessage;
        if(isMimiced) {
            ssFailMessage << "mimiced ";
        }
        if(isChaosed) {
            ssFailMessage << "chaosed ";
        }
        ssFailMessage << this->delegate.abilityIdToString(aid) << " ";
        if(factionId > 0) {
            ssFailMessage << factionIdToString(factionId) << " ";
        }
        ssFailMessage << effectArgument;
        ssFailMessage << " failed because there is no target";
        std::string failMessage(ssFailMessage.str());
        this->ability(eventCondition,src,failMessage);
    }
}

void DeckLogger::abilityFailDisease(EVENT_CONDITION const & eventCondition
                                   ,AbilityId const & aid
                                   ,PlayedCard const & src
                                   ,PlayedCard const & target
                                   ,bool const & isMimiced
                                   ,FactionId const & factionId
                                   ,EFFECT_ARGUMENT const & effectArgument
                                   )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY_FAILED)) {
        std::stringstream ssFailMessage;
        if(isMimiced) {
            ssFailMessage << "mimiced ";
        }
        ssFailMessage << this->delegate.abilityIdToString(aid) << " ";
        if(factionId > 0) {
            ssFailMessage << factionIdToString(factionId) << " ";
        }
        ssFailMessage << effectArgument;
        ssFailMessage << " failed for target " << target.toString() << " because it is diseased";
        std::string failMessage(ssFailMessage.str());
        this->ability(eventCondition,src,failMessage);
    }
}
