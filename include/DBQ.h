#ifndef DBQ_H
#define DBQ_H

#include <string>
#include <mariadb/conncpp.hpp>

#include <Config.h>
#include <Logging.h>

class DBQ{
    private:
        sql::Driver* m_driver = nullptr;
        sql::SQLString m_url = nullptr;
        sql::Properties m_properties;

        std::unique_ptr<sql::Connection> m_conn;

        Config& config = Config::get_instance();
        Logging m_logger;
        
    public:
        DBQ();
        ~DBQ();

        int get_device_id(std::string devname);
        void insert_devname(std::string devname);
        void get_last_device_entry(int deviceid, nlohmann::json &json);
        void insert_history(double temp, double humidity, int device_id, const std::string& datetime);
};
#endif