#ifndef ITERATEDECKS_CACHE_SQLITEWRAPPER_HPP
    #define ITERATEDECKS_CACHE_SQLITEWRAPPER_HPP

    #include <string>
    #include <map>
    #include <vector>
    #include <sqlite3.h>
    #include <set>

    namespace IterateDecks {
        namespace Cache {

            class SQLResult {
                private:
                    std::map<std::string,std::string> data;

                public:
                    void put(std::string const & name, std::string const & data);
                    unsigned long getULong(std::string const & column) const;
            };
            
            typedef std::vector<SQLResult> SQLResults;

            typedef std::string Statement;

            class SQLiteWrapper;

            class PreparedStatement {
                private:
                    sqlite3_stmt * statement;
                    
                public:
                    PreparedStatement(sqlite3_stmt *);
                    ~PreparedStatement();

                    void bindText(unsigned int index, std::string text);
                    void bindInt(unsigned int index, int value);
                    void bindNull(unsigned int index);
                    void execute();
                    SQLResults query();
            };

            class SQLiteWrapper {
                private:
                    sqlite3 * database;
                    std::set<PreparedStatement *> statements;

                public:
                    SQLiteWrapper(std::string const & fileName);
                    ~SQLiteWrapper();

                    PreparedStatement * prepareStatement(Statement const & statement);
                    void execute(Statement const &);
                    
            };
            
        }
    }
        
#endif
