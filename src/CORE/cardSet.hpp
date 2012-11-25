#ifndef ITERATEDECKS_CORE_CARDSET_HPP
    #define ITERATEDECKS_CORE_CARDSET_HPP

    namespace IterateDecks {
        namespace Core {

            struct CardSet {
                public:
                    std::string name;
                    std::string icon;
                    bool visible;

                public:
                    CardSet()
                    : visible(false)
                    {
                    }

                    CardSet(char const *name, char const * icon, bool visible)
                    : name(name)
                    , icon(icon)
                    , visible(visible)
                    {
                    }
            };

            typedef std::map<UINT, CardSet> MSETS;
            typedef std::pair<UINT, CardSet> PAIRMSETS;

        }
    }
#endif

