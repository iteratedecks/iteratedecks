#include "diskBackedCache.hpp"
#include <sstream>
#include <iostream>
#include <ctime>

using namespace IterateDecks::Core;
namespace IterateDecks {
    namespace Cache {

        DiskBackedCache::DiskBackedCache(SimulatorCore::Ptr & delegate)
        : SimulatorCache(delegate)
        , ignoreCoreVersion(true)
        , ignoreXMLVersion(true)
        , dontReadFromCache(false)
        , database("cache.sqlite")
        {
            std::stringstream ssCreateTable;
            ssCreateTable << "CREATE TABLE IF NOT EXISTS ";
            ssCreateTable << "IDCache";
            ssCreateTable << " (" << "coreName string NOT NULL";
            ssCreateTable << ", " << "coreVersion string NOT NULL";
            ssCreateTable << ", " << "xmlVersions string NOT NULL";
            ssCreateTable << ", " << "attacker string NOT NULL";
            ssCreateTable << ", " << "defender string NOT NULL";
            ssCreateTable << ", " << "surge bool NOT NULL";
            ssCreateTable << ", " << "delayFirstCard bool NOT NULL";
            ssCreateTable << ", " << "battleGroundId int DEFAULT 0";
            ssCreateTable << ", " << "achievementId int DEFAULT -1";
            ssCreateTable << ", " << "numberOfGames int NOT NULL";
            ssCreateTable << ", " << "gamesWon int NOT NULL";
            ssCreateTable << ", " << "gamesStalled int NOT NULL";
            ssCreateTable << ", " << "gamesLost int NOT NULL";
            ssCreateTable << ", " << "pointsAttacker int NOT NULL";
            ssCreateTable << ", " << "pointsAttackerAuto int NOT NULL";
            ssCreateTable << ", " << "pointsDefender int NOT NULL";
            ssCreateTable << ", " << "pointsDefenderAuto int NOT NULL";
            ssCreateTable << ")";
            database.execute(ssCreateTable.str());

            std::stringstream ssCreateIndex;
            ssCreateIndex << "CREATE INDEX IF NOT EXISTS ";
            ssCreateIndex << "task";
            ssCreateIndex << " ON ";
            ssCreateIndex << "IDCache";
            ssCreateIndex << " (" << "attacker";
            ssCreateIndex << " ," << "defender";
            ssCreateIndex << " ," << "surge";
            ssCreateIndex << " ," << "delayFirstCard";
            ssCreateIndex << " ," << "battleGroundId";
            ssCreateIndex << " ," << "achievementId";
            ssCreateIndex << ")";
            database.execute(ssCreateIndex.str());

            std::stringstream ssInsert;
            ssInsert << "INSERT INTO " << "IDCache ";
            ssInsert << "(coreName, coreVersion, xmlVersions";
            ssInsert << ", attacker, defender, surge, delayFirstCard";
            ssInsert << ", battleGroundId, achievementId";
            ssInsert << ", numberOfGames, gamesWon, gamesStalled, gamesLost";
            ssInsert << ", pointsAttacker, pointsAttackerAuto, pointsDefender, pointsDefenderAuto";
            ssInsert << ") ";
            ssInsert << "VALUES ";
            ssInsert << "(?, ?, ?";
            ssInsert << ", ?, ?, ?, ?";
            ssInsert << ", ?, ?";
            ssInsert << ", ?, ?, ?, ?";
            ssInsert << ", ?, ?, ?, ?";
            ssInsert << ")";
            this->insertStatement = database.prepareStatement(ssInsert.str());

            this->insertStatement->bindText(1, this->delegate->getCoreName());
            this->insertStatement->bindText(2, this->delegate->getCoreVersion());
            std::stringstream xmlVersion;
            std::map<std::string,std::string> xmlHashes = this->delegate->getXMLVersions();
            for(std::map<std::string,std::string>::const_iterator iter = xmlHashes.begin()
               ; iter != xmlHashes.end()
               ; iter++
               )
            {
                // TODO: Order is probably not guaranteed, need to address that
                xmlVersion << iter->first << "=" << iter->second << ";";
            }
            this->insertStatement->bindText(3, xmlVersion.str());

            std::stringstream ssSelect;
            ssSelect << "SELECT " << std::endl;
            ssSelect << "\tnumberOfGames, gamesWon, gamesStalled, gamesLost";
            ssSelect << ", pointsAttacker, pointsAttackerAuto, pointsDefender, pointsDefenderAuto";
            ssSelect << " " << std::endl;
            ssSelect << "FROM " << "IDCache " << std::endl;
            ssSelect << "WHERE coreName = ? ";
            ssSelect << "AND (coreVersion = ? ";
            if(this->ignoreCoreVersion) {
                ssSelect << "OR 1) ";
            }
            ssSelect << "AND (xmlVersions = ? ";
            if(this->ignoreXMLVersion) {
                ssSelect << "OR 1) ";
            }
            ssSelect << "AND attacker = ? ";
            ssSelect << "AND defender = ? ";
            ssSelect << "AND surge = ? ";
            ssSelect << "AND delayFirstCard = ? ";
            ssSelect << "AND battleGroundId = ? ";
            ssSelect << "AND achievementId = ? ";
            ssSelect << ";";
            this->selectStatement = database.prepareStatement(ssSelect.str());
            this->selectStatement->bindText(1, this->delegate->getCoreName());
            this->selectStatement->bindText(2, this->delegate->getCoreVersion());
            this->selectStatement->bindText(3, xmlVersion.str());

            this->randomData = static_cast<unsigned int>(time(0));
        }
        
        DiskBackedCache::~DiskBackedCache()
        {
        }

        Result
        DiskBackedCache::loadCache(SimulationTaskClass const & task
                                  )
        {
            if(this->dontReadFromCache) {
                return Result();
            }
            this->selectStatement->bindText(4, task.attacker->toString());
            this->selectStatement->bindText(5, task.defender->toString());
            this->selectStatement->bindInt(6, task.surge);
            this->selectStatement->bindInt(7, task.delayFirstAttacker);
            this->selectStatement->bindInt(8, static_cast<int>(task.battleGround));
            if (task.achievementOptions.isEnabled()) {
                this->selectStatement->bindInt(9, task.achievementOptions);
            } else {
                this->selectStatement->bindInt(9, -1);
            }
            SQLResults sqlResults = this->selectStatement->query();
            Result result;

            for(SQLResults::const_iterator iter = sqlResults.begin()
               ;iter != sqlResults.end()
               ;iter++
               )
            {
                SQLResult const & sqlResult = *iter;
                Result currentResult;
                currentResult.numberOfGames      = sqlResult.getULong("numberOfGames");
                currentResult.gamesWon           = sqlResult.getULong("gamesWon");
                currentResult.gamesStalled       = sqlResult.getULong("gamesStalled");
                currentResult.gamesLost          = sqlResult.getULong("gamesLost");
                currentResult.pointsAttacker     = sqlResult.getULong("pointsAttacker");
                currentResult.pointsAttackerAuto = sqlResult.getULong("pointsAttackerAuto");
                currentResult.pointsDefender     = sqlResult.getULong("pointsDefender");
                currentResult.pointsDefenderAuto = sqlResult.getULong("pointsDefenderAuto");
                result += currentResult;
                //std::clog << "loaded entry with " << currentResult.numberOfGames << " elements" << std::endl;
            }
            //std::clog << "loaded cache with " << result.numberOfGames << " elements" << std::endl;
            //if(result.gamesWon < result.numberOfGames) {
            //    throw Exception("Non perfect result (this is expected and only a debugging exception.)");
            //}
            return result;
        }

        void
        DiskBackedCache::addToCache(SimulationTaskClass const & task
                                   ,Result const & freshResult
                                   )
        {

            this->insertStatement->bindText(4, task.attacker->toString());
            this->insertStatement->bindText(5, task.defender->toString());
            this->insertStatement->bindInt(6, task.surge);
            this->insertStatement->bindInt(7, task.delayFirstAttacker);
            this->insertStatement->bindInt(8, static_cast<int>(task.battleGround));
            if (task.achievementOptions.isEnabled()) {
                this->insertStatement->bindInt(9, task.achievementOptions);
            } else {
                this->insertStatement->bindInt(9, -1);
            }
            this->insertStatement->bindInt(10, freshResult.numberOfGames);
            this->insertStatement->bindInt(11, freshResult.gamesWon);
            this->insertStatement->bindInt(12, freshResult.gamesStalled);
            this->insertStatement->bindInt(13, freshResult.gamesLost);
            this->insertStatement->bindInt(14, freshResult.pointsAttacker);
            this->insertStatement->bindInt(15, freshResult.pointsAttackerAuto);
            this->insertStatement->bindInt(16, freshResult.pointsDefender);
            this->insertStatement->bindInt(17, freshResult.pointsDefenderAuto);
            this->insertStatement->execute();
        }

        Result
        DiskBackedCache::simulate(SimulationTaskClass const & task
                                 )
        {
            Result cached = loadCache(task);
            if (cached.numberOfGames < task.minimalNumberOfGames) {
                unsigned long missingGames = task.minimalNumberOfGames - cached.numberOfGames;
                SimulationTaskClass remainderTask(task);
                remainderTask.minimalNumberOfGames = missingGames;
                remainderTask.randomSeed = rand_r(&(this->randomData));
                Result fresh = this->delegate->simulate(remainderTask);
                this->addToCache(remainderTask, fresh);
                return cached + fresh;
            } else {
                return cached;
            }
        }


        Result
        DiskBackedCache::simulate(SimulationTaskClass const & task
                                 ,unsigned long numberOfNewSamples
                                 )
        {
            Result cached = loadCache(task);
            if (cached.numberOfGames < task.minimalNumberOfGames) {
                unsigned long missingGames = task.minimalNumberOfGames - cached.numberOfGames;
                unsigned long newGames = std::max(missingGames, numberOfNewSamples);
                SimulationTaskClass remainderTask(task);
                remainderTask.minimalNumberOfGames = newGames;
                Result fresh = this->delegate->simulate(remainderTask);
                this->addToCache(remainderTask, fresh);
                return cached + fresh;
            } else {
                return cached;
            }
        }

        void
        DiskBackedCache::abort()
        {
            this->delegate->abort();
        }

        void
        DiskBackedCache::setDontReadCache(bool dontReadCache)
        {
            this->dontReadFromCache = dontReadCache;
        }

    }
}
