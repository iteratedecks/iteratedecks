/*
 * This is not a source file, do not compile it directly!
 */
/*
 * This is not a simple header file, do not include it directly!
 * (unless you are assert.hpp in the same folder)
 */

#include <sstream>
#include <iomanip>

namespace Assert {

    template<typename T> inline
    std::string xtos(T const & x)
    {
        std::stringstream ss;
        ss << x;
        return ss.str();
    }

    template<> inline
    std::string xtos(double const & d) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(16) << d;
        return ss.str();
    }

    template<> inline
    std::string xtos(unsigned char const & c) {
        std::stringstream ss;
        ss << (unsigned int)c;
        return ss.str();
    }

    template<typename T> inline
    AssertionError assertGT_(T const & a
                            ,T const & b
                            ,std::string const & sa
                            ,std::string const & sb
                            ,std::string const & file
                            ,unsigned int const line
                            ,std::string const & function
                            ,std::string const & msg
                            )
    {
        return AssertionError(msg + sa + " == " + xtos(a) + " !> " + xtos(b) + " == " + sb,file,line,function);
    }

    template<typename T> inline
    AssertionError assertGE_(T const & a
                            ,T const & b
                            ,std::string const & sa
                            ,std::string const & sb
                            ,std::string const & file
                            ,unsigned int const line
                            ,std::string const & function
                            ,std::string const & msg
                            )
    {
        return AssertionError(msg + "; " + sa + " == " + xtos(a) + " !>= " + xtos(b) + " == " + sb,file,line,function);
    }

    template<typename T> inline
    AssertionError assertLT_(T const & a
                            ,T const & b
                            ,std::string const & sa
                            ,std::string const & sb
                            ,std::string const & file
                            ,unsigned int const line
                            ,std::string const & function
                            ,std::string const & msg
                            )
    {
        return AssertionError(msg + "; "  + sa + " == " + xtos(a) + " !< " + xtos(b) + " == " + sb,file,line,function);
    }

    template<typename T> inline
    AssertionError assertLE_(T const & a
                            ,T const & b
                            ,std::string const & sa
                            ,std::string const & sb
                            ,std::string const & file
                            ,unsigned int const line
                            ,std::string const & function
                            ,std::string const & msg
                            )
    {
        return AssertionError(msg + "; "  + sa + " == " + xtos(a) + " !<= " + xtos(b) + " == " + sb,file,line,function);
    }

    template<typename T> inline
    AssertionError assertEQ_(T const & a
                            ,T const & b
                            ,std::string const & sa
                            ,std::string const & sb
                            ,std::string const & file
                            ,unsigned int const line
                            ,std::string const & function
                            ,std::string const & msg
                            )
    {
        return AssertionError(msg + "; "  + sa + " == " + xtos(a) + " !== " + xtos(b) + " == " + sb,file,line,function);
    }

}
