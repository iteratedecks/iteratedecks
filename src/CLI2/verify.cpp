#include "verify.hpp"

#include <sstream>

VerifyOptions::VerifyOptions()
: doVerify(false)
, min(0.0d)
, max(1.0d)
{}
    
//VerifyOptions::VerifyOptions(double const min
//                            ,double const max
//                            )
//: doVerify(true)
//, min(min)
//, max(max)
//{}

VerifyOptions::VerifyOptions(char const * const str) {
    this->doVerify = true;
    std::stringstream ssStr(str);
    ssStr >> this->min;
    char colon;
    ssStr >> colon;
    if (colon != ':') {
        throw IncorrectVerifySyntax();
    }
    ssStr >> this->max;
    if (ssStr.fail()) {
        throw IncorrectVerifySyntax();
    }
}
