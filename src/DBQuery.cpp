#include <iostream>
#include <sstream>

#include <DBQuery.h>
#include <mysql.h>

DBQuery::DBQuery(){
    MYSQL *conn;
    if (!(conn = mysql_init(0))){
        m_logger.log(Logging::severity_level::warning, std::string("Unable to initialize connection", "GENTRACE"), "GENTRACE");
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
        CLIENT_MULTI_STATEMENTS
    )){
        std::stringstream ss;
        ss << "Error connecting to db server: " << mysql_error(m_conn);
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
        mysql_close(m_conn);
    }
}

DBQuery::~DBQuery(){
    mysql_close(m_conn);
}

void DBQuery::query(std::string query){
    // test connection here

    if(mysql_query(m_conn, query.c_str())){
        m_logger.log(Logging::severity_level::warning, std::string("Error running query"), "GENTRACE");
    }

    MYSQL_RES *result = mysql_store_result(m_conn);

    if (result == NULL)
    {
        m_logger.log(Logging::severity_level::warning, std::string("Error storing query result"), "GENTRACE");
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
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
        return 1;
    }

    return 0;
}

int DBQuery::select(std::string query, std::string output){
    MYSQL_RES *result;

    if(mysql_real_query(m_conn, query.c_str(), query.length())){
        std::stringstream ss;
        ss << "Error running query: " << query;
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
        return 1;
    }

    result = mysql_use_result(m_conn);
    if(result == NULL){
        return 1;
    }

    MYSQL_ROW row;
    std::string tmp_result;

    while ((row = mysql_fetch_row(result))) {
        // Iterate through columns in the current row
        for (int i = 0; i < mysql_num_fields(result); ++i) {
            if (row[i]) { // Check if the field is not NULL
                tmp_result += std::string(row[i]);
            } else {
                tmp_result += "NULL"; // Handle NULL values
            }
            if (i < mysql_num_fields(result) - 1) {
                tmp_result += "\t"; // Add a tab delimiter between fields
            }
        }
        tmp_result += "\n"; // Add a newline after each row
    }
    output = tmp_result;

    mysql_free_result(result);

    return 0;
}

int DBQuery::get_last_insert_id(){
    return mysql_insert_id(m_conn);
}