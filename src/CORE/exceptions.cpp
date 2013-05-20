#include "exceptions.hpp"
#include <iomanip>
#include <sstream>
#include <string>

#ifdef WITH_BACKTRACE
    #include <execinfo.h>
    #include <dlfcn.h>
    #include <cxxabi.h>
    #include <cstdlib>
    #include <pstream.h>
#endif

// class Exception taken from http://markus-mazurczak.de/?p=17 under GPL v3
Exception::Exception(std::string const & msg)
: _msg(msg)
#ifdef WITH_BACKTRACE
, skipFirstSymbols(1)
#endif
{
    #ifdef WITH_BACKTRACE
        _traced = backtrace(_stacktrace, 64);
    #endif
}

Exception::Exception(std::string const & msg, unsigned int const skipSymbols)
: _msg(msg)
#ifdef WITH_BACKTRACE
, skipFirstSymbols(1+skipSymbols)
#endif
{
	#ifdef WITH_BACKTRACE
        _traced = backtrace(_stacktrace, 64);
    #endif
}

std::string Exception::getError() const throw()
{
	return(_msg);
}

const char *Exception::what() const throw()
{
	return(_msg.c_str());
}

void Exception::printStacktrace(std::ostream & os) const throw() {
    #ifdef WITH_BACKTRACE
        unsigned int const wIndex = 2;
        unsigned int const wSharedObject = 40;
        unsigned int const wFunction = 70;

        if(_traced <= 0) {
            os << "Error: No stacktrace available" << std::endl;
        } else {
            os << "Stacktrace available, most recent call on top: " << (_traced-skipFirstSymbols) << " items" << std::endl;
            os << std::setw(wIndex) << std::left << "#" << " ";
            os << std::setw(wSharedObject) << "shared object" << " ";
            os << std::setw(wFunction) << "function" << " ";
            os << "file:line";
            os << std::internal << std::endl;
            for(unsigned int i = skipFirstSymbols; i < (unsigned int)_traced; i++) {
                os << std::setw(wIndex) << (i-skipFirstSymbols) << " ";
                Dl_info dlinfo;
                if(!dladdr(_stacktrace[i], &dlinfo)) {
                    os << "failed to resolve name and file for " << _stacktrace[i] << " this is probably due to a static build." << std::endl;
                } else {
                    char const * const sharedObjectName = dlinfo.dli_fname;
                    //void * const sharedObjectAddress = dlinfo.dli_fbase;
                    char const * symName = dlinfo.dli_sname;
                    //void * const symAddress = dlinfo.dli_saddr;
                    char* demangled;
                    int status;
                    demangled = abi::__cxa_demangle(symName, NULL, 0, &status);
                    if(status == 0 && demangled) {
                        symName = demangled;
                    }
                    os << std::setw(wSharedObject) << std::left << sharedObjectName << " ";
                    os << std::setw(wFunction) << std::left;
                    os << ((symName != NULL) ? symName : "<not applicable>");
                    os << std::internal << " ";
                    if(demangled) {
                        free(demangled);
                    }
                    #ifdef BACKTRACE_SECOND_VARIANT
                        std::vector<std::string> addr2lineArgs;
                        addr2lineArgs.push_back("addr2line");
                        std::stringstream ssPtr;
                        ssPtr << (void *)(_stacktrace[i]);
                        addr2lineArgs.push_back(ssPtr.str());
                        addr2lineArgs.push_back("-e");
                        addr2lineArgs.push_back(sharedObjectName);
                        redi::ipstream syscom("addr2line", addr2lineArgs);
                        std::string str;
                        while(syscom >> str) {
                            os << str;
                        }
                    #endif
                    os <<  std::endl;
                }
            } // for
        }
    #else
        os << "Stacktrace not available. Not supported on your platform?" << std::endl;
        os << "Thats YOUR chance to do implement it." << std::endl;
    #endif
}

LogicError::LogicError(std::string const & what)
: std::logic_error(what)
, Exception(what)
{
}

LogicError::LogicError(std::string const & what, unsigned int const skipSymbols)
: std::logic_error(what)
, Exception(what, 1 + skipSymbols)
{
}

RuntimeError::RuntimeError(std::string const & what)
: std::runtime_error(what)
, Exception(what)
{
}

RuntimeError::RuntimeError(std::string const & what, unsigned int const skipSymbols)
: std::runtime_error(what)
, Exception(what,1+skipSymbols)
{
}

InvalidUserInputError::InvalidUserInputError(std::string const & what)
: RuntimeError(what,1)
{
}

InvalidDeckHashError::InvalidDeckHashError(std::string const & what)
: InvalidUserInputError(what)
{
}

std::string const InvalidDeckHashError::notEvenChars("Deck hashes should have even length.");


InvalidState::InvalidState(std::string const & what)
: LogicError(what)
{
}
