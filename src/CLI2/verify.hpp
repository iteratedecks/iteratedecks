
#include <exception>

struct VerifyOptions {
    public:
        bool doVerify;
        double min;
        double max;
    
    public:    
        VerifyOptions();    
//        VerifyOptions(double const min, double const max);
        VerifyOptions(char const * const str);

};

class IncorrectVerifySyntax: public std::exception {
};
