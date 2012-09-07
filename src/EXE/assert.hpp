#ifndef ASSERT_HPP_
#define ASSERT_HPP_

    #ifndef NDEBUG
        #include <stdexcept>
        #include <string>
        //#include <cassert>

        #define assertX(b) do { if(!(b)) { throw AssertionError(#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
        #define assertDEE(a,b,c) do { if(std::fabs(a-b) > c) { throw AssertionError(a,b,c,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)
        #define assertGE(a,b) do { if(!(a>=b)) { throw assertGE_(a,b,#a,#b,__FILE__,__LINE__,FUNCTION_NAME); } } while(false)

        #if defined(__GNUC__)
            #define FUNCTION_NAME __PRETTY_FUNCTION__
        #else
            #define FUNCTION_NAME "<unknown>"
        #endif

        class AssertionError : public std::logic_error {
            public:
                AssertionError(std::string message
                              ,std::string file
                              ,unsigned int line
                              ,std::string function
                              );

                AssertionError(double const a
                              ,double const b
                              ,double const delta
                              ,std::string file
                              ,unsigned int line
                              ,std::string function
                              );
        };

        AssertionError assertGE_(double a, double b, std::string sa, std::string sb, std::string file, unsigned int line, std::string function);

    #endif
#endif
