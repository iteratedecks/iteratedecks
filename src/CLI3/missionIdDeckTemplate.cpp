#include "missionIdDeckTemplate.hpp"
#include "../CORE/activeDeck.hpp"
#include "../CORE/cardDB.hpp"
#include <boost/lexical_cast.hpp>

namespace IterateDecks {
    namespace CLI3 {

        MissionIdDeckTemplate::MissionIdDeckTemplate(unsigned int missionId)
        : missionId(missionId)
        {
        }

        ActiveDeck
        MissionIdDeckTemplate::instantiate(CardDB const & cardDB) const
        {
            return cardDB.GetMissionDeck(this->missionId);
        }

        std::string
        MissionIdDeckTemplate::toString() const
        {
            return "MISSIONID:" + boost::lexical_cast<std::string>(this->missionId);
        }


        DeckTemplate::Ptr MissionIdDeckTemplate::withCommander(unsigned int commanderId) const { throw Exception("Not supported!"); }
        size_t MissionIdDeckTemplate::getNumberOfNonCommanderCards() const { throw Exception("Not supported!"); }
        unsigned int MissionIdDeckTemplate::getCardAtIndex(size_t index) const { throw Exception("Not supported!"); }
        DeckTemplate::Ptr MissionIdDeckTemplate::withCardAtIndex(unsigned int cardId, size_t index) const { throw Exception("Not supported!"); }
        DeckTemplate::Ptr MissionIdDeckTemplate::withoutCardAtIndex(size_t index) const { throw Exception("Not supported!"); }
        unsigned int MissionIdDeckTemplate::getCommander() const { throw Exception("Not supported!"); }
        DeckTemplate::Ptr MissionIdDeckTemplate::swapCards(size_t i, size_t j) const { throw Exception("Not supported!"); }
    }
}
