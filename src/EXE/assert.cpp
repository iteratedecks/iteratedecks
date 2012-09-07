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

AssertionError::AssertionError(std::string message
                              ,std::string file
                              ,unsigned int line
                              ,std::string function
                              )
: std::logic_error(message + " in file " + file + ":" + itos(line) + "(" + function + ")")
{
}


AssertionError::AssertionError(double const a
                              ,double const b
                              ,double const delta
                              ,std::string file
                              ,unsigned int line
                              ,std::string function
                              )
: std::logic_error("doubles not close enough a=" +dtos(a) + ", b=" + dtos(b) + ", delta=" + dtos(delta) + " in file " + file + ":" + itos(line) + "(" + function + ")")
{
}


AssertionError assertGE_(double a
              ,double b
              ,std::string sa
              ,std::string sb
              ,std::string file
              ,unsigned int line
              ,std::string function
              )
{
    return AssertionError(sa + "== " + dtos(a) + " >= " + dtos(b) + " == " + sb,file,line,function);
}
