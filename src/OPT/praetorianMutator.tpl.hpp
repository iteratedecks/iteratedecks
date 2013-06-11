/*
 * This is not a source file, do not compile it directly!
 */
/*
 * This is not a simple header file, do not include it directly!
 * (unless you are the corresponding .hpp in the same folder)
 */

#include "../CORE/card.hpp"
#include "../CORE/cardDB.hpp"
#include "../CORE/assert.hpp"

namespace IterateDecks {
    namespace Opt {
        
        template < class InputIterator
                 >
        void
        PraetorianMutator::initAllowedCardsFromCollection(InputIterator begin
                                                         ,InputIterator const & end
                                                         )                                                         
        {
            for(InputIterator iter = begin
               ;iter != end
               ;iter++
            ){
                unsigned int const cardId = *iter;
                if(!this->isExcluded(cardId)) {
                    IterateDecks::Core::Card const & card = cardDB.GetPointer()[cardId];
                    assertX(card.GetType() != IterateDecks::Core::TYPE_NONE);
                    if(card.GetType() == IterateDecks::Core::TYPE_COMMANDER) {
                        this->allowedCommanders.insert(cardId);
                    } else {
                        this->allowedNonCommanderCards.insert(cardId);
                    }
                }
            } // for
        }

    }
}
