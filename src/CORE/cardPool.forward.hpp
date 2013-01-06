#ifndef ITERATEDECKS_CORE_CARDPOOL_FORWARD_HPP
    #define ITERATEDECKS_CORE_CARDPOOL_FORWARD_HPP

    #include <vector>

    namespace IterateDecks {
        namespace Core {

            struct CardPool;
            typedef std::vector<CardPool> VCARDPOOL;

            // FIXME these should not be defines
            #define DEFAULT_POOL_COUNT	5
            #define DEFAULT_POOL_SIZE	10
        }
    }

#endif
