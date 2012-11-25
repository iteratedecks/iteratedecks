#ifndef ITERATEDECKS_CORE_PLAYEDCARD_FORWARD_HPP
    #define ITERATEDECKS_CORE_PLAYEDCARD_FORWARD_HPP

    #include <list>
    #include <vector>
    #include "simpleTypes.hpp"

    namespace IterateDecks {
        namespace Core {

            class PlayedCard;

            // TODO the name seems misleading
            typedef std::list<PlayedCard> LCARDS;
            typedef std::vector<PlayedCard> VCARDS;
            typedef std::vector<PlayedCard*> PVCARDS;
            typedef std::pair<PlayedCard*,UCHAR> PPCARDINDEX;
            typedef std::vector<PPCARDINDEX> PPCIV;
        }
    }

#endif

