#include "cliOptions.hpp"

DeckArgument::DeckArgument()
: type(HASH)
{
}

DeckArgument::DeckArgumentType DeckArgument::getType() const
{
    return this->type;
}

std::string DeckArgument::getHash() const throw (InvalidState)
{
    if(this->type != HASH) {
        throw InvalidState("Only hash decks have a hash.");
    }
    return this->hashData.hash;
}

bool DeckArgument::isOrdered() const throw (InvalidState)
{
    if(this->type != HASH) {
        throw InvalidState("Only hash decks can be ordered.");
    }
    return this->hashData.ordered;
}

void DeckArgument::setOrdered(bool const & ordered)
{
    if(this->type != HASH) {
        throw InvalidState("Only hash decks can be ordered.");
    }
    this->hashData.ordered = ordered;
}

void DeckArgument::setHash(std::string const & hash)
{
    this->type = HASH;
    this->hashData.hash = hash;
}


AchievementOptions::AchievementOptions()
: doAchievementCheck(false)
, achievementIndex(0)
{
}

void AchievementOptions::enableCheck(unsigned int const & achievementIndex)
{
    this->doAchievementCheck = true;
    this->achievementIndex = achievementIndex;
}

void AchievementOptions::disableCheck()
{
    this->doAchievementCheck = false;
    this->achievementIndex = 0;
}

AchievementOptions::operator int() const
{
    if(this->doAchievementCheck) {
        return this->achievementIndex;
    } else {
        return -1;
    }
}

CliOptions::CliOptions()
: numberOfIterations(DEFAULT_NUMBER_OF_ITERATIONS)
, verbosity(0)
{
}
