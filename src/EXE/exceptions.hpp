#ifndef EXCEPTIONS_HPP_
    #define EXCEPTIONS_HPP_

    #include <stdexcept>

    class InvalidUserInputError : public std::runtime_error {
        public:
            explicit InvalidUserInputError(std::string const & what);
    };

    class InvalidDeckHashError : public InvalidUserInputError {
        public:
            explicit InvalidDeckHashError(std::string const & what);

            static std::string const notEvenChars;
    };


#endif
