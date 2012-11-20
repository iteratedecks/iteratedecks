#include "missionInfo.hpp"

#include "constants.hpp"

#include "compat.h"

#include <sstream>
#include <string>

namespace IterateDecks {
    namespace Core {

        MissionInfo::MissionInfo()
        : Commander(0)
        {
        }

        MissionInfo::MissionInfo(UINT const commander, const char * const name)
        : Commander(commander)
        {
            if (name) {
                Name = std::string(name);
            }
            Deck.reserve(DEFAULT_DECK_RESERVE_SIZE); // Deck is a vector of integers, I am quite convinced we can skip that
        }

        void MissionInfo::Add(UINT const cardID)
        {
            Deck.push_back(cardID);
        }

        std::string MissionInfo::GetDeck() const
        {
            std::stringstream ss;
            bool first = true;
            for(VID::const_iterator iter = this->Deck.begin(); iter != this->Deck.end(); iter++) {
                UINT const index = *iter;
                if (!first) {
                    ss << ",";
                } else {
                    first = false;
                }
                ss << index;
            }
            return ss.str();
        }

        UINT MissionInfo::GetCommander() const
        {
            return this->Commander;
        }

        std::string MissionInfo::GetName() const
        {
            return this->Name;
        }

        UINT MissionInfo::GetCardCount() const
        {
            return static_cast<UINT>(this->Deck.size());
        }

        UINT MissionInfo::GetCardID(UINT Index) const
        {
            return this->Deck[Index];
        }

        MissionInfo::~MissionInfo()
        {
            //Commander = 0; // pointless
            //Deck.clear(); // pointless
        }

        bool MissionInfo::IsValid() const
        {
            return this->Commander != 0;
        }
    }
}
