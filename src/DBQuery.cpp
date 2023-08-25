#include <iostream>

#include <DBQuery.h>
#include <mysql.h>

DBQuery::DBQuery(){
    MYSQL *conn;
    if (!(conn = mysql_init(0))){
        std::cout << "unable to initialize connection"  << std::endl;
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
        std::cout << "Error connecting to db server: " << mysql_error(m_conn)  << std::endl;
        mysql_close(m_conn);
    }
}

DBQuery::~DBQuery(){
    mysql_close(m_conn);
}

void DBQuery::query(std::string query){
    // test connection here

    if(mysql_query(m_conn, query.c_str())){
        std::cout << "error running query"  << std::endl;
    }

    MYSQL_RES *result = mysql_store_result(m_conn);

    if (result == NULL)
    {
        std::cout << "Error storing query result"  << std::endl;
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

void DBQuery::insert(std::string query){
    // Create logic to check if it's an insert

    // End logic

    if(mysql_query(m_conn, query.c_str())){
        std::cout << "error running query: " << query << std::endl;
    }
}

int DBQuery::get_last_insert_id(){
    return mysql_insert_id(m_conn);
}