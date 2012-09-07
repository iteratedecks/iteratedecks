#include "assert.hpp"
#include <sstream>

std::string itos(unsigned int const i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

std::string dtos(double const d) {
    std::stringstream ss;
    ss << d;
    return ss.str();
}

AssertionError::AssertionError(std::string const & message
                              ,std::string const & file
                              ,unsigned int const line
                              ,std::string const & function
                              )
: LogicError(message + " in file " + file + ":" + itos(line) + "(" + function + ")", 3)
{
}

AssertionError assertGE_(double const a
                        ,double const b
                        ,std::string const & sa
                        ,std::string const & sb
                        ,std::string const & file
                        ,unsigned int const line
                        ,std::string const & function
                        )
{
    return AssertionError(sa + "== " + dtos(a) + " >= " + dtos(b) + " == " + sb,file,line,function);
}

AssertionError assertDEE_(double const a
                         ,double const b
                         ,double const delta
                         ,std::string const & file
                         ,unsigned int const line
                         ,std::string const & function
                         )
{
    return AssertionError("doubles not close enough a=" +dtos(a) + ", b=" + dtos(b) + ", delta=" + dtos(delta),file,line,function);
}
