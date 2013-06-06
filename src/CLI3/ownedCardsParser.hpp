#ifndef ITERATEDECKS_CLI3_OWNEDCARDSPARSER_HPP
    #define ITERATEDECKS_CLI3_OWNEDCARDSPARSER_HPP

    #include <set>
    #include <string>
    #include <istream>

    namespace IterateDecks {
        namespace Core {

            std::multiset<unsigned int>
            parseSimpleSeparatedString(std::istream & is
                                      ,std::string const & rowSeparator
                                      ,std::string const & columnSeparator
                                      );
            
        }
    }
    
#endif
