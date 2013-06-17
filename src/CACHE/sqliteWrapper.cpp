#include "sqliteWrapper.hpp"
#include "../CORE/exceptions.hpp"
#include <sstream>
#include "../CORE/assert.hpp"
#include <boost/lexical_cast.hpp>

namespace IterateDecks {
    namespace Cache {

        unsigned long
        SQLResult::getULong(std::string const & column) const
        {
            try {
                std::string value = this->data.at(column);
                return boost::lexical_cast<unsigned long>(value);
            } catch (std::out_of_range & e) {
                std::cerr << "Trying to get value for name " << column << " from map with " << std::endl;
                for(std::map<std::string,std::string>::const_iterator iter = this->data.begin()
                   ;iter != this->data.end()
                   ;iter++)
                {
                    std::cerr << iter->first << "=" << iter->second << std::endl;
                }
                throw LogicError("");
            }
        }

        void
        SQLResult::put(std::string const & name, std::string const & data)
        {
            this->data[name] = data;
        }

        PreparedStatement::PreparedStatement(sqlite3_stmt * statement)
        : statement(statement)
        {
            assertX(this->statement != NULL);            
        }

        PreparedStatement::~PreparedStatement()
        {
            sqlite3_finalize(this->statement);
        }


        void
        PreparedStatement::bindText(unsigned int index
                                   ,std::string value
                                   )
        {
            assertX(this->statement != NULL);
            int resultCode = sqlite3_bind_text(this->statement, index, value.c_str(), -1, SQLITE_TRANSIENT);
            if (resultCode != SQLITE_OK) {
                std::stringstream ssMessage;
                ssMessage << "Cannot bind parameter index ";
                ssMessage << index << " with value " << value;
                ssMessage << " to prepared statement";
                ssMessage << ", error code " << resultCode << ".";
                throw Exception(ssMessage.str());
            }
        }
        void
        PreparedStatement::bindInt(unsigned int index
                                  ,int value
                                  )
        {
            assertX(this->statement != NULL);            
            int resultCode = sqlite3_bind_int(this->statement, index, value);
            if (resultCode != SQLITE_OK) {
                std::stringstream ssMessage;
                ssMessage << "Cannot bind parameter index ";
                ssMessage << index << " with value " << value;
                ssMessage << " to prepared statement";
                ssMessage << ", error code " << resultCode << ".";
                throw Exception(ssMessage.str());
            }
        }
        void
        PreparedStatement::bindNull(unsigned int index
                                   )
        {
            assertX(this->statement != NULL);            
            int resultCode = sqlite3_bind_null(this->statement, index);
            if (resultCode != SQLITE_OK) {
                std::stringstream ssMessage;
                ssMessage << "Cannot bind parameter index ";
                ssMessage << index << " to null";
                ssMessage << " to prepared statement";
                ssMessage << ", error code " << resultCode << ".";
                throw Exception(ssMessage.str());
            }
        }

        void
        PreparedStatement::execute()
        {
            int resultCode = sqlite3_step(this->statement);
            if (resultCode != SQLITE_DONE) {
                std::stringstream ssMessage;
                ssMessage << "Cannot execute prepared statement";
                ssMessage << ", error code " << resultCode << ".";
                throw Exception(ssMessage.str());
            }
            resultCode = sqlite3_reset(this->statement);
            if (resultCode != SQLITE_OK) {
                std::stringstream ssMessage;
                ssMessage << "Cannot reset prepared statement";
                ssMessage << ", error code " << resultCode << ".";
                throw Exception(ssMessage.str());
            }
        }

        SQLResults
        PreparedStatement::query()
        {
            unsigned int const columnCount = sqlite3_column_count(this->statement);
            SQLResults sqlResults;
            do {
                int resultCode = sqlite3_step(this->statement);
                if (resultCode == SQLITE_DONE) {
                    resultCode = sqlite3_reset(this->statement);
                    if (resultCode != SQLITE_OK) {
                        std::stringstream ssMessage;
                        ssMessage << "Cannot reset prepared statement";
                        ssMessage << ", error code " << resultCode << ".";
                        throw Exception(ssMessage.str());
                    }
                    return sqlResults;
                } else if (resultCode != SQLITE_ROW) {
                    std::stringstream ssMessage;
                    ssMessage << "Cannot query prepared statement";
                    ssMessage << ", error code " << resultCode << ".";
                    ssMessage << "Query was:" << std::endl;
                    ssMessage << sqlite3_sql(this->statement);
                    throw Exception(ssMessage.str());
                }
                //std::clog << "Got a chunk from database!" << std::endl;
                SQLResult sqlResult;
                for(unsigned int i = 0; i < columnCount; i++) {
                    std::string data = reinterpret_cast<char const *>(sqlite3_column_text(this->statement, i));
                    std::string name = sqlite3_column_name(this->statement, i);
                    sqlResult.put(name,data);
                }
                sqlResults.push_back(sqlResult);
            } while (true);

            throw Exception("This shouldn't run. It fixes a compiler warning.");
        }


        SQLiteWrapper::SQLiteWrapper(std::string const & fileName)
        {
            int resultCode = sqlite3_open(fileName.c_str(), &(this->database));
            if (resultCode != SQLITE_OK) {
                std::stringstream ssMessage;
                ssMessage << "Cannot open sqlite database '";
                ssMessage << fileName << "', error code " << resultCode << ".";
                throw Exception(ssMessage.str());
            }
        }

        SQLiteWrapper::~SQLiteWrapper()
        {
            for(std::set<PreparedStatement *>::iterator iter = this->statements.begin()
               ;iter != this->statements.end()
               ;iter++
               )
            {
                delete *iter;
            }
            sqlite3_close(this->database);
        }

        PreparedStatement *
        SQLiteWrapper::prepareStatement(Statement const & statement)
        {
            sqlite3_stmt *pStmt;
            int resultCode = sqlite3_prepare_v2(this->database, statement.c_str(), -1, &pStmt, NULL);
            if (resultCode != SQLITE_OK) {
                std::stringstream ssMessage;
                ssMessage << "Cannot prepare statement " << std::endl;
                ssMessage << statement << std::endl;
                ssMessage << ", error code " << resultCode << ".";
                throw Exception(ssMessage.str());
            }
            if (pStmt == NULL) {
                std::stringstream ssMessage;
                ssMessage << "Got no statement back for SQL" << std::endl;
                ssMessage << statement << std::endl;
                ssMessage << "and no error either.";
                throw Exception(ssMessage.str());
            }
            assertX(pStmt != NULL);
            PreparedStatement * preparedStatement = new PreparedStatement(pStmt);
            this->statements.insert(preparedStatement);
            return preparedStatement;
        }

        void
        SQLiteWrapper::execute(Statement const & statement)
        {
            char * cErrorMessage;
            int rc = sqlite3_exec(this->database, statement.c_str(), NULL, 0, &cErrorMessage);
            if (rc != SQLITE_OK) {
                std::string errorMessage(cErrorMessage);
                sqlite3_free(cErrorMessage);
                throw Exception("SQLite3 Error: " + errorMessage);
            }
        }
        
    }
}
    
