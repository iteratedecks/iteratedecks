#include "Logger.hpp"

DeckLogger::DeckLogger(DeckType const & deckType, Logger const & delegate)
: delegate(delegate)
, deckType(deckType)
{
}
