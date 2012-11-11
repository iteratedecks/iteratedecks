#ifndef ASSERT_HPP_
#define ASSERT_HPP_

    #ifndef NDEBUG
        #include "exceptions.hpp"
        #include <string>
        #include <cmath>
        //#include <cassert>

        #define assertX(b) do { if(!(b)) { throw AssertionError(#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
        #define assertDEE(a,b,c) do { if(std::fabs(a-b) > c) { throw assertDEE_(a,b,c,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
        #define assertGE(a,b) do { if(!(a>=b)) { throw assertGE_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
        #define assertLT(a,b) do { if(!(a<b )) { throw assertLT_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
        #define assertLE(a,b) do { if(!(a<=b )) { throw assertLE_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)

        #if defined(__GNUC__)
            #define FUNCTION_NAME __PRETTY_FUNCTION__
        #else
            #define FUNCTION_NAME "<unknown>"
        #endif

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

    #endif
#endif
