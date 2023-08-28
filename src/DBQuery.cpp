#include <iostream>
#include <sstream>

#include <DBQuery.h>
#include <mysql.h>

DBQuery::DBQuery(){
    MYSQL *conn;
    if (!(conn = mysql_init(0))){
        m_logger.log_trace("Unable to initialize connection", "GENTRACE");
    }
    else{
        m_conn = conn;
    }

    if(!mysql_real_connect(
        m_conn,
        config.GET_DB_IP().c_str(),
        config.GET_DB_USERNAME().c_str(),
        config.GET_DB_PASSWORD().c_str(),
        config.GET_DB_NAME().c_str(),
        config.GET_DB_PORT(),
        NULL,
        0
    )){
        std::stringstream ss;
        ss << "Error connecting to db server: " << mysql_error(m_conn);
        m_logger.log_trace(ss.str(), "GENTRACE");
        mysql_close(m_conn);
    }
}

DBQuery::~DBQuery(){
    mysql_close(m_conn);
}

void DBQuery::query(std::string query){
    // test connection here

    if(mysql_query(m_conn, query.c_str())){
        m_logger.log_trace("Error running query", "GENTRACE");
    }

    MYSQL_RES *result = mysql_store_result(m_conn);

    if (result == NULL)
    {
        m_logger.log_trace("Error storing query result", "GENTRACE");
    }


    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        for(int i = 0; i < num_fields; i++)
        {
            std::cout << row[i] << " ";
        }
        std::cout << std::endl;
    }

    mysql_free_result(result);
}

int DBQuery::insert(std::string query){
    // Create logic to check if it's an insert

    // End logic

    if(mysql_query(m_conn, query.c_str())){
        std::stringstream ss;
        ss << "Error running query: " << query;
        m_logger.log_trace(ss.str(), "GENTRACE");
        return 1;
    }
    
    return 0;
}

int DBQuery::get_last_insert_id(){
    return mysql_insert_id(m_conn);
}