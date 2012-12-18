#ifndef VERIFY_HPP_
#define VERIFY_HPP_
#include <stdexcept>

struct VerifyOptions {
    public:
        bool doVerify;
        double winMin;
        double winMax;
        double drawMin;
        double drawMax;
        double lossMin;
        double lossMax;
    
    public:    
        VerifyOptions();    
//        VerifyOptions(double const min, double const max);
        VerifyOptions(char const * const str);

};

class IncorrectVerifySyntax: public std::runtime_error {
    public:
        IncorrectVerifySyntax();
};
#endif
