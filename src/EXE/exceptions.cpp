#include "exceptions.hpp"

InvalidUserInputError::InvalidUserInputError(std::string const & what)
: runtime_error(what)
{
}

InvalidDeckHashError::InvalidDeckHashError(std::string const & what)
: InvalidUserInputError(what)
{
}

std::string const InvalidDeckHashError::notEvenChars("Deck hashes should have even length.");
