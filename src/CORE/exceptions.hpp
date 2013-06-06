#ifndef EXCEPTIONS_HPP_
    #define EXCEPTIONS_HPP_

    #include <stdexcept>
    #include <ostream>

    #if defined(__GNUC__) && defined(__linux__)
        #define WITH_BACKTRACE
        #define BACKTRACE_SECOND_VARIANT
    #endif

    // class Exception taken from http://markus-mazurczak.de/?p=17 under GPL v3
    class Exception : public std::exception {
        private:
            std::string _msg;
            #ifdef WITH_BACKTRACE
                int _traced;
                unsigned int skipFirstSymbols;
                void *_stacktrace[64];
            #endif
        public:
            /** \fn Exception(std::string msg)
             * \brief Constructor awaiting your error message as a std::string
             * @param msg Error description
             *
             * \todo Implementing option to specify maximal stacktracing depth
             */
            Exception(std::string const & msg);

            Exception(std::string const & msg, unsigned int const skipFirstSymbols);

            /** \fn virtual ~Exception() throw() {}
             * \brief Virtual standard destructor who must be disallowed to throw exceptions
             */
            virtual ~Exception() throw() {};

            /** \fn virtual std::string getError(void) const throw()
             * \brief Returning the last error message and itself is disallowed to throw a exception
             * @return std::string
             */
            virtual std::string getError(void) const throw();

            /** \fn virtual const char *what(void) const throw()
             * \brief Polymorphed what function from standard C++ exception class retunring the last error message
             * 		  and itself is disallowed to throw a exception
             * @return const char *
             */
            virtual const char *what(void) const throw();

            /** \fn void printStacktrace(void) const throw()
             * \brief Printing out a stacktrace and itself is disallowed to throw a exception
             * @param os stream to print the stacktrace to
             */
            void printStacktrace(std::ostream & os) const throw();
    };

    class LogicError : public std::logic_error, public Exception {
        public:
            using Exception::what;

            LogicError(std::string const & what);
            LogicError(std::string const & what, unsigned int const skipFirstSymbols);
            virtual ~LogicError() throw() {};
    };

    class RuntimeError : public std::runtime_error, public Exception {
        public:
            using Exception::what;

            RuntimeError(std::string const & what);
            RuntimeError(std::string const & what, unsigned int const skipFirstSymbols);
            virtual ~RuntimeError() throw() {};
    };

    class InvalidUserInputError : public RuntimeError {
        public:
            explicit InvalidUserInputError(std::string const & what);
    };

    class InvalidState : public LogicError {
        public:
            explicit InvalidState(std::string const & what);
    };

    class AbortionException : public RuntimeError {
        public:
            explicit AbortionException(std::string const & what);
    };

#endif
