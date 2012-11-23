#ifndef RESULTS_HPP_
    #define RESULTS_HPP_

    #include "deck.hpp"

    namespace IterateDecks { namespace Core {

    typedef unsigned long       DWORD;

    struct RESULTS
    {
        DWORD Win;
        DWORD Loss;
        DWORD Games;
        DWORD Points;
        DWORD AutoPoints;
        DWORD LPoints;
        DWORD LAutoPoints;
        RESULTS();
        void Add(const RESULTS rAdd);
    };

}}
#endif
