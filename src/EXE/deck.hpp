#ifndef _DECK_HPP_
    #define _DECK_HPP_

    #include <vector>
    #include <list>
    #include <set>

    typedef	unsigned char UCHAR;
    typedef	unsigned int UINT;
    typedef UINT EFFECT_ARGUMENT;         // that is usally the amount (i.e, the 3 in weaken 3), but also the card to summon in summon

    class Card;
    class ActiveDeck;
    class PlayedCard;

    typedef std::vector<PlayedCard> VCARDS;
    typedef std::vector<PlayedCard*> PVCARDS;
    typedef std::pair<PlayedCard*,UCHAR> PPCARDINDEX;
    typedef std::vector<PPCARDINDEX> PPCIV;
    typedef std::multiset<UINT> MSID;
    typedef std::list<PlayedCard> LCARDS;

    /**
     * Conditions for an event, pseudo bit feld, value 0 is special.
     */
    typedef unsigned char EVENT_CONDITION;

#endif
