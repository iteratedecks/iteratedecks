#include "assert.hpp"

namespace Assert {

    AssertionError::AssertionError(std::string const & message
                                  ,std::string const & file
                                  ,unsigned int const line
                                  ,std::string const & function
                                  )
    : LogicError(message + " in file " + file + ":" + xtos(line) + "(" + function + ")", 3)
    {
    }

}

#ifndef INLINE
    #include "assert.inline.hpp"
#endif
