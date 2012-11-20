/*
 * This is not a source file, do not compile it directly!
 */
/*
 * This is not a simple header file, do not include it directly!
 * (unless you are assert.hpp in the same folder)
 */

#ifdef INLINE
    #define MAYBE_INLINE inline
#else
    #define MAYBE_INLINE
#endif

namespace Assert {

    MAYBE_INLINE
    AssertionError assertDEE_(double const a
                             ,double const b
                             ,double const delta
                             ,std::string const & sa
                             ,std::string const & sb
                             ,std::string const & file
                             ,unsigned int const line
                             ,std::string const & function
                             )
    {
        return AssertionError("doubles not close enough "+sa+"=" + xtos(a) + ", " + sb + "=" + xtos(b) + ", delta=" + xtos(delta), file, line, function);
    }
}
