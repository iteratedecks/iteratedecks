#ifndef ITERATEDECKS_CORE_MISSIONINFO_HPP
    #define ITERATEDECKS_CORE_MISSIONINFO_HPP

    #include <string>
    #include "simpleTypes.hpp"

    namespace IterateDecks {
        namespace Core {

            class MissionInfo {
                private:
                    std::string Name;
                    UINT Commander;
                    VID Deck;
                public:
                    MissionInfo();
                    MissionInfo(UINT const commander, char const * const name = 0);
                    void Add(UINT const cardID);
                    std::string GetDeck() const;
                    UINT GetCommander() const;
                    std::string GetName() const;
                    UINT GetCardCount() const;
                    UINT GetCardID(UINT Index) const;
                    ~MissionInfo();
                    bool IsValid() const;
            };

        }
    }

#endif
