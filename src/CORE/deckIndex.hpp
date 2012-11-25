#ifndef ITERATEDECKS_CORE_DECKINDEX_HPP
    #define ITERATEDECKS_CORE_DECKINDEX_HPP

    #include "deckIndex.forward.hpp"

    namespace IterateDecks {
        namespace Core {

            struct DeckIndex {
                public:
                    std::string name;
                    Tag tag;

                public:
                    DeckIndex(std::string const & name)
                    : name(name)
                    , tag(TAG_CUSTOM)
                    {
                    }

                    DeckIndex() {}

                    DeckIndex(std::string const & name, Tag const tag)
                    : name(name)
                    , tag(tag)
                    {
                    }

                    ~DeckIndex() {}

                    bool operator<(DeckIndex const & deckIndex) const
                    {
                        if (this->name < deckIndex.name) {
                            return true;
                        } else if (this->name > deckIndex.name) {
                            return false;
                        } else {
                            return this->tag < deckIndex.tag;
                        }
                    }
            };
        }
    }
#endif

