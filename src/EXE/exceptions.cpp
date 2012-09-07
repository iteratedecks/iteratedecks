#include "exceptions.hpp"
#include <iomanip>

#ifdef WITH_BACKTRACE
    #include <execinfo.h>
    #include <dlfcn.h>
    #include <cxxabi.h>
    #include <cstdlib>
#endif

// class Exception taken from http://markus-mazurczak.de/?p=17 under GPL v3
Exception::Exception(std::string const & msg)
: _msg(msg)
, skipFirstSymbols(0)
{
	_traced = backtrace(_stacktrace, 64);
}

Exception::Exception(std::string const & msg, unsigned int const skipFirstSymbols)
: _msg(msg)
, skipFirstSymbols(skipFirstSymbols)
{
	_traced = backtrace(_stacktrace, 64);
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

        if(_traced <= 0) {
            os << "Error: No stacktrace available" << std::endl;
        } else {
            os << "Stacktrace available, most recent call on top: " << (_traced-skipFirstSymbols) << " items" << std::endl;
            os << std::setw(wIndex) << std::left << "#" << " ";
            os << std::setw(wSharedObject) << "shared object" << " -- ";
            os << "function";
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
                    os << std::setw(wSharedObject) << std::left << sharedObjectName << " -- ";
                    os << ((symName != NULL) ? symName : "<not applicable>");
                    os << std::internal <<  std::endl;
                    if(demangled) {
                        free(demangled);
                    }
                }
            }
        }
    #else
        os << "Stacktrace not available. Not supported on your platform?" << std::endl;
    #endif
}

LogicError::LogicError(std::string const & what)
: std::logic_error(what)
, Exception(what)
{
}

LogicError::LogicError(std::string const & what, unsigned int const skipFirstSymbols)
: std::logic_error(what)
, Exception(what, skipFirstSymbols)
{
}

RuntimeError::RuntimeError(std::string const & what)
: std::runtime_error(what)
, Exception(what)
{
}


InvalidUserInputError::InvalidUserInputError(std::string const & what)
: RuntimeError(what)
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
