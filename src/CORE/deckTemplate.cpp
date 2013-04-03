#include "deckTemplate.hpp"
//#include "activeDeck.hpp"
//#include "cardDB.hpp"
//#include "assert.hpp"
//#include "cardPool.hpp"

//#include <iostream>

namespace IterateDecks {
    namespace Core {

        DeckTemplate::DeckTemplate(CardDB const & cardDB)
        : commander(0)
        , cardDB(cardDB)
        , allowInvalid(false)
        {}

        DeckTemplate::DeckTemplate(Card const * commander, CardDB const & cardDB)
        : commander(commander)
        , cardDB(cardDB)
        , allowInvalid(false)
        {}

        DeckTemplate::~DeckTemplate() {}

        BattleGroundEffect DeckTemplate::getBattleGroundEffect() const
        {
            return BattleGroundEffect::normal;
        }
        
    }
}
