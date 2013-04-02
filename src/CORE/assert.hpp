#ifndef ITERATEDECKS_CORE_ASSERT_HPP
#define ITERATEDECKS_CORE_ASSERT_HPP

    /**
     * This module provides assertion macros for easy assertion testing.
     * They will be defined if NDEBUG is not defined. (Otherwise they
     * will still be defined but resolve to an empty expression.)
     */

    #include <string>
    #include "exceptions.hpp"
    #include <cmath>

    // This should return the name of the function. Unfortunately
    // this is compiler dependant.
    #if defined(__GNUC__)
        #define FUNCTION_NAME __PRETTY_FUNCTION__
    #else
        #define FUNCTION_NAME "<unknown>"
    #endif

        #define DO_WRAP(x) do { x } while(false)

    /**
         * Assert binary relation of two values with optional message
         * @param a first value
         * @param b second value
         * @param m a mesasge
         * @throws AssertionError
         */
        #define assertGTM(a,b,m) DO_WRAP( if(!(a> b)) { throw Assert::assertGT_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME, m); })
        #define assertGEM(a,b,m) DO_WRAP( if(!(a>=b)) { throw Assert::assertGE_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME, m); })
        #define assertLTM(a,b,m) DO_WRAP( if(!(a< b)) { throw Assert::assertLT_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME, m); })
        #define assertLEM(a,b,m) DO_WRAP( if(!(a<=b)) { throw Assert::assertLE_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME, m); })
        #define assertEQM(a,b,m) DO_WRAP( if(!(a==b)) { throw Assert::assertEQ_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME, m); })

        /**
        * A namespace for the functions so they don't clutter global
        * namespace.
        * (macros ignore namespaces, to indicate that they are defined
        * outside the namespace)
        */
    namespace Assert {

        /**
            * A logic error thrown if an assertion fails.
            */
        class AssertionError : public LogicError {
            public:
                AssertionError(std::string const & message
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const &function
                                );
        };

        /**
            * Assertion helper functions.
            */
        template<typename T>
        AssertionError assertGT_(T const & a
                                ,T const & b
                                ,std::string const & sa
                                ,std::string const & sb
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const & function
                                    ,std::string const & msg = std::string()
                                );
        template<typename T>
        AssertionError assertGE_(T const & a
                                ,T const & b
                                ,std::string const & sa
                                ,std::string const & sb
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const & function
                                    ,std::string const & msg = std::string()
                                );
        template<typename T>
        AssertionError assertLT_(T const & a
                                ,T const & b
                                ,std::string const & sa
                                ,std::string const & sb
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const & function
                                    ,std::string const & msg = std::string()
                                );
        template<typename T>
        AssertionError assertLE_(T const & a
                                ,T const & b
                                ,std::string const & sa
                                ,std::string const & sb
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const & function
                                    ,std::string const & msg = std::string()
                                );
        template<typename T>
        AssertionError assertEQ_(T const & a
                                ,T const & b
                                ,std::string const & sa
                                ,std::string const & sb
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const & function
                                    ,std::string const & msg = std::string()
                                );
        AssertionError assertDEE_(double const a
                                    ,double const b
                                    ,double const delta
                                    ,std::string const & sa
                                    ,std::string const & sb
                                    ,std::string const & file
                                    ,unsigned int const line
                                    ,std::string const & function
                                    );

        /**
            * Conmversion helper functions
            */
        template<typename T>
        std::string xtos(T const & x);

    } // namespace

    // Unfortunetaly template methods need to be in the header :(
    #include "assert.tpl.hpp"
    #ifdef INLINE
        // similar for inline methods
        #include "assert.inline.hpp"
    #endif

    #ifndef NDEBUG
        /**
         * Asserts that a condition is true.
         * @param b the condition
         * @throws AssertionErrror if the condition is false
         */
        #define assertX(b) do { if(!(b)) { throw Assert::AssertionError(#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)

        /**
         * Assert that two doubles are close to each other.
         * @param a first double
         * @param b second double
         * @param eps maximum allowed difference
         * @throws AssertionError if the doubles are not that close.
         */
        #define assertDEE(a,b,eps) do { if(std::fabs(a-b) > eps) { throw Assert::assertDEE_(a,b,eps,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)

        /**
         * Assert binary relation of two values
         * @param a first value
         * @param b second value
         * @throws AssertionError
         */
        #define assertGT(a,b) do { if(!(a> b)) { throw Assert::assertGT_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
        #define assertGE(a,b) do { if(!(a>=b)) { throw Assert::assertGE_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
        #define assertLT(a,b) do { if(!(a< b)) { throw Assert::assertLT_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
        #define assertLE(a,b) do { if(!(a<=b)) { throw Assert::assertLE_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
        #define assertEQ(a,b) do { if(!(a==b)) { throw Assert::assertEQ_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
    #else
        #define assertX
        #define assertDEE
        #define assertGT
        #define assertGE
        #define assertLT
        #define assertLE
        #define assertEQ
    #endif
#endif
