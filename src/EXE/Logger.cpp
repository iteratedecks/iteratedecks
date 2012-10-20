#include "Logger.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include "assert.hpp"

static int logInit=0;
unsigned long const Logger::LOG_NONE                     (0);
unsigned long const Logger::LOG_ABILITY                  (1<<logInit++);
unsigned long const Logger::LOG_ABILITY_FAILED           (1<<logInit++);
unsigned long const Logger::LOG_ABILITY_FAILED_NOTARGET  (1<<logInit++);
unsigned long const Logger::LOG_ATTACK                   (1<<logInit++);
unsigned long const Logger::LOG_ATTACK_BEGIN_END         (1<<logInit++);
unsigned long const Logger::LOG_TURN                     (1<<logInit++);
unsigned long const Logger::LOG_SIMULATION               (1<<logInit++);
unsigned long const Logger::LOG_ALL                      ((1<<logInit)-1);

enum Color {
    GREEN,
    LIGHT_GREEN,
    LIGHT_BLUE
};

std::string ansiColor(Color const & color)
{
    std::stringstream colorCode;
    colorCode << "\033[";
    switch(color) {
        case GREEN: colorCode << "32"; break;
        case LIGHT_GREEN: colorCode << "1;32"; break;
        case LIGHT_BLUE: colorCode << "1;34"; break;
    }
    colorCode << "m";
    return colorCode.str();
}

std::string ansiNoColor()
{
    return "\033[22;39m";
}

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

Logger::Logger(unsigned long const & flags, CardDB const * const & cDB)
: flags(flags)
, cDB(cDB)
, colorMode(COLOR_NONE)
{
}

void Logger::setColorMode(ColorMode const & colorMode) 
{
    this->colorMode = colorMode;
}

std::string Logger::colorCard(PlayedCard const & playedCard) const
{
    assertX(playedCard.IsDefined());
    switch (this->colorMode) {
        case COLOR_NONE:
            return playedCard.toString();
        case COLOR_ANSI:
            // ansi escape sequences do look like arcane magic, but don't be afraid
            return ansiColor(LIGHT_BLUE) + playedCard.toString() + ansiNoColor();
        default:
            throw std::logic_error("switch fail, not implemented?");
    }
}

std::string Logger::colorTurn(std::string const & str) const
{
    switch (this->colorMode) {
        case COLOR_NONE:
            return str;
        case COLOR_ANSI:
            // ansi escape sequences do look like arcane magic, but don't be afraid
            return ansiColor(GREEN) + str + ansiNoColor();
        default:
            throw std::logic_error("switch fail, not implemented?");
    }
}

std::string Logger::colorSimulation(std::string const & str) const
{
    switch (this->colorMode) {
        case COLOR_NONE:
            return str;
        case COLOR_ANSI:
            // ansi escape sequences do look like arcane magic, but don't be afraid
            return ansiColor(LIGHT_GREEN) + str + ansiNoColor();
        default:
            throw std::logic_error("switch fail, not implemented?");
    }
}



void Logger::log(unsigned long const & flags
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

bool Logger::isEnabled(unsigned long const & flags) const
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
        this->delegate.log(Logger::LOG_TURN,colorTurn(message));
    }
}

void SimulationLogger::turnEnd(unsigned int const & turn)
{
    if (this->delegate.isEnabled(Logger::LOG_TURN)) {
        std::stringstream ssMessage;
        ssMessage << "Turn " << turn << " ends";
        std::string message(ssMessage.str());
        this->delegate.log(Logger::LOG_TURN,colorTurn(message));
    }
}

void SimulationLogger::simulationStart(unsigned int const & simulation)
{
    if (this->delegate.isEnabled(Logger::LOG_SIMULATION)) {
        std::stringstream ssMessage;
        ssMessage << "Simulation " << simulation << " starts";
        std::string message(ssMessage.str());
        this->delegate.log(Logger::LOG_SIMULATION,colorSimulation(message));
    }
}

void SimulationLogger::simulationEnd(unsigned int const & simulation)
{
    if (this->delegate.isEnabled(Logger::LOG_SIMULATION)) {
        std::stringstream ssMessage;
        ssMessage << "Simulation " << simulation << " ends";
        std::string message(ssMessage.str());
        this->delegate.log(Logger::LOG_SIMULATION,colorSimulation(message));
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
        std::stringstream ssMessage;
        ssMessage << eventConditionToString(eventCondition);
        //ssMessage << this->getDeckStr();
        ssMessage << colorCard(src) << " ";
        ssMessage << message;
        this->delegate.log(Logger::LOG_ABILITY,ssMessage.str());
    }
}

void DeckLogger::attack(PlayedCard const & src
                              ,std::string const & message
                              )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY)) {
        //std::string deckStr = this->getDeckStr();
        this->delegate.log(Logger::LOG_ATTACK,colorCard(src) + " " + message);
    }
}

void DeckLogger::abilitySummon(EVENT_CONDITION const & eventCondition
                              ,PlayedCard const & src
                              ,PlayedCard const & summonedCard
                              )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY)) {
        std::string summonMessage("summons " + colorCard(summonedCard));
        this->ability(eventCondition,src,summonMessage);
    }
}

void DeckLogger::abilityTribute(EVENT_CONDITION const & eventCondition
                               ,PlayedCard const & tributer
                               ,PlayedCard const & tributee
                               ,AbilityId const & abilityId
                               ,EFFECT_ARGUMENT const & effectArgument
                               )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY)) {
        std::stringstream ssMessage;
        ssMessage << " tributes " << colorCard(tributee) << " ";
        ssMessage << this->delegate.abilityIdToString(abilityId);
        ssMessage << " " << (unsigned int)effectArgument;
        this->ability(eventCondition,tributer,ssMessage.str());
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
        this->attack(src,flurryMessage);
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
        assertX(src.IsDefined());
        assertX(target.IsDefined());
        std::stringstream ssMessage;
        ssMessage << this->delegate.abilityIdToString(aid) << " ";
        ssMessage << (unsigned int)amount << " ";
        ssMessage << colorCard(target);
        this->ability(eventCondition,src,ssMessage.str());
    }
}

void DeckLogger::abilityOffensive(EVENT_CONDITION const & eventCondition
                                 ,PlayedCard const & src
                                 ,AbilityId const & aid
                                 ,PlayedCard const & target
                                 ,EFFECT_ARGUMENT amount
                                 ,bool evaded
                                 )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY)) {
        std::stringstream ssMessage;
        ssMessage << this->delegate.abilityIdToString(aid) << " ";
        ssMessage << (unsigned int)amount << " ";
        ssMessage << colorCard(target);
        if(evaded) {
            ssMessage << " evaded";
        }
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
        ssFailMessage << " failed for target " << colorCard(target) << " because it is diseased";
        std::string failMessage(ssFailMessage.str());
        this->ability(eventCondition,src,failMessage);
    }
}

void DeckLogger::abilityFailNoProc(EVENT_CONDITION const & eventCondition
                                  ,PlayedCard const & src
                                  ,AbilityId const & aid
                                  ,PlayedCard const & target
                                  )
{
    if (this->delegate.isEnabled(Logger::LOG_ABILITY_FAILED)) {
        std::stringstream ssFailMessage;
        ssFailMessage << this->delegate.abilityIdToString(aid) << " ";
        ssFailMessage << " failed to tribute " << colorCard(target) << " because it did not proc";
        std::string failMessage(ssFailMessage.str());
        this->ability(eventCondition,src,failMessage);
    }
}

std::string DeckLogger::colorCard(PlayedCard const & playedCard) const
{
    return this->delegate.colorCard(playedCard);
}

std::string SimulationLogger::colorTurn(std::string const & str) const
{
    return this->delegate.colorTurn(str);
}

std::string SimulationLogger::colorSimulation(std::string const & str) const
{
    return this->delegate.colorSimulation(str);
}


void DeckLogger::attackBegin(PlayedCard const & attacker) {
    if(this->delegate.isEnabled(Logger::LOG_ATTACK_BEGIN_END)) {
        std::stringstream ssMessage;
        ssMessage << colorCard(attacker);
        ssMessage << " begins attack";
        this->delegate.log(Logger::LOG_ATTACK_BEGIN_END,ssMessage.str());
    }
}

void DeckLogger::attackSwipe(PlayedCard const & attacker)
{
    if(this->delegate.isEnabled(Logger::LOG_ATTACK)) {
        std::stringstream ssMessage;
        ssMessage << colorCard(attacker);
        ssMessage << " swipes";
        this->delegate.log(Logger::LOG_ATTACK,ssMessage.str());
    }
}


void DeckLogger::attackTarget(PlayedCard const & attacker
                             ,PlayedCard const & target
                             )
{
    if(this->delegate.isEnabled(Logger::LOG_ATTACK)) {
        std::stringstream ssMessage;
        ssMessage << "tries to attack ";
        ssMessage << colorCard(target);
        ssMessage << " ...";
        this->attack(attacker, ssMessage.str());
    }
}

void DeckLogger::attackBonus(PlayedCard const & attacker
                            ,PlayedCard const & target
                            ,AbilityId abilityId
                            ,EFFECT_ARGUMENT amount
                            )
{
    if(this->delegate.isEnabled(Logger::LOG_ATTACK)) {
        std::stringstream ssMessage;
        ssMessage << "gains ";
        ssMessage << this->delegate.abilityIdToString(abilityId);
        ssMessage << " ";
        ssMessage << (unsigned int)amount;
        ssMessage << " against ";
        ssMessage << colorCard(target);
        this->attack(attacker,ssMessage.str());
    }
}


void DeckLogger::attackHit(PlayedCard const & attacker, PlayedCard const & victim, unsigned int const & damage) {
    if(this->delegate.isEnabled(Logger::LOG_ATTACK)) {
        std::stringstream ssMessage;
        ssMessage << "attacks ";
        ssMessage << colorCard(victim);
        ssMessage << " for " << damage << " damage";
        this->attack(attacker, ssMessage.str());
    }
}

void DeckLogger::attackEnd(PlayedCard const & attacker) {
    if(this->delegate.isEnabled(Logger::LOG_ATTACK_BEGIN_END)) {
        this->attack(attacker, "ends attack");
    }
}

void DeckLogger::defensiveAbility(PlayedCard const & victim
                                 ,PlayedCard const & attacker
                                 ,AbilityId abilityId
                                 ,EFFECT_ARGUMENT amount
                                 )
{
    if(this->delegate.isEnabled(Logger::LOG_ATTACK)) {
        std::stringstream ssMessage;
        ssMessage << "has ";
        ssMessage << this->delegate.abilityIdToString(abilityId);
        ssMessage << " ";
        ssMessage << (unsigned int)amount;
        ssMessage << " against ";
        ssMessage << colorCard(attacker);
        this->attack(victim,ssMessage.str());
    }
}
