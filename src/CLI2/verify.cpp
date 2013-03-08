#include "verify.hpp"

#include <sstream>

#define VERIFY_RANGE_DELIM ':'
#define VERIFY_PARAM_DELIM ','

VerifyOptions::VerifyOptions()
: doVerify(false)
, winMin(0.0)
, winMax(1.0)
, drawMin(0.0)
, drawMax(1.0)
, lossMin(0.0)
, lossMax(1.0)
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

    ssStr >> this->winMin;
    char delim = NULL;
    ssStr >> delim;
    if (delim != VERIFY_RANGE_DELIM) {
        throw IncorrectVerifySyntax();
    }
    ssStr >> this->winMax;

    if(!ssStr.eof()) {
        delim = NULL;
        ssStr >> delim;
        if (delim != VERIFY_PARAM_DELIM) {
            throw IncorrectVerifySyntax();
        }

        ssStr >> this->drawMin;
        delim = NULL;
        ssStr >> delim;
        if (delim != VERIFY_RANGE_DELIM) {
            throw IncorrectVerifySyntax();
        }
        ssStr >> this->drawMax;
    } else {
        this->drawMin = 0;
        this->drawMax = 1;
    }

    if(!ssStr.eof()) {
        delim = NULL;
        ssStr >> delim;
        if (delim != VERIFY_PARAM_DELIM) {
            throw IncorrectVerifySyntax();
        }

        ssStr >> this->lossMin;
        delim = NULL;
        ssStr >> delim;
        if (delim != VERIFY_RANGE_DELIM) {
            throw IncorrectVerifySyntax();
        }
        ssStr >> this->lossMax;
    } else {
        this->lossMin = 0;
        this->lossMax = 1;
    }

    if (ssStr.fail()) {
        throw IncorrectVerifySyntax();
    }
}

IncorrectVerifySyntax::IncorrectVerifySyntax()
: runtime_error("Incorrect verify syntax")
{
}
