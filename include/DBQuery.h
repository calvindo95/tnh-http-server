#ifndef DBQUERY_H
#define DBQUERY_H

#include <string>
#include <mysql.h>

#include <Config.h>
#include <Logging.h>

class DBQuery{
    private:
        MYSQL *m_conn;
        Config& config = Config::get_instance();
        Logging m_logger;
        
    public:
        DBQuery();
        ~DBQuery();

        int insert(std::string query);

        void query(std::string query);

        int get_last_insert_id();
};
#endif