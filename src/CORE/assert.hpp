#ifndef ASSERT_HPP_
#define ASSERT_HPP_

    /**
     * This module provides assertion macros for easy assertion testing.
     * They will be defined if NDEBUG is not defined. (Otherwise they
     * will still be defined but resolve to an empty expression.)
     */

    #ifndef NDEBUG
        #include "exceptions.hpp"
        #include <string>
        #include <cmath>

        // This should return the name of the function. Unfortunately
        // this is compiler dependant.

        #if defined(__GNUC__)
            #define FUNCTION_NAME __PRETTY_FUNCTION__
        #else
            #define FUNCTION_NAME "<unknown>"
        #endif

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

        class AssertionError : public LogicError {
            public:
                AssertionError(std::string const & message
                              ,std::string const & file
                              ,unsigned int const line
                              ,std::string const &function
                              );
        };

        AssertionError assertGE_(double const a
                                ,double const b
                                ,std::string const & sa
                                ,std::string const & sb
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const & function
                                );
        AssertionError assertLT_(double const a
                                ,double const b
                                ,std::string const & sa
                                ,std::string const & sb
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const & function
                                );
        AssertionError assertLT_(unsigned int const a
                                ,unsigned int const b
                                ,std::string const & sa
                                ,std::string const & sb
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const & function
                                );
        AssertionError assertLE_(double const a
                                ,double const b
                                ,std::string const & sa
                                ,std::string const & sb
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const & function
                                );
        AssertionError assertLE_(unsigned int const a
                                ,unsigned int const b
                                ,std::string const & sa
                                ,std::string const & sb
                                ,std::string const & file
                                ,unsigned int const line
                                ,std::string const & function
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

    #else
        #define assertX
        #define assertDEE
        #define assertGT
        #define assertGE
        #define assertLT
        #define assertLE
    #endif
#endif
