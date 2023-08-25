#ifndef DBQUERY_H
#define DBQUERY_H

#include <string>
#include <mysql.h>

#include <Config.h>

class DBQuery{
    private:
        MYSQL *m_conn;
        Config& config = Config::get_instance();
        
    public:
        DBQuery();
        ~DBQuery();

        void insert(std::string query);

        void query(std::string query);

        int get_last_insert_id();
};
#endif