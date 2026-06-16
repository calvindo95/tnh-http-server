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
        void insert_history(double temp, double humidity, std::string devicename, const std::string& datetime);

        std::string list_device_count();
        nlohmann::json list_device_count_json();

        void get_last_device_data(int deviceid, nlohmann::json& json);

        nlohmann::json get_device_history(int deviceid, const std::string& start, const std::string& end, int bucket_minutes);

        // Returns user_id (>0) if found, 0 if not found, -1 on error
        int get_user(const std::string& username, std::string& out_password_hash);
        bool create_session(int user_id, const std::string& session_id);

        // Returns new user_id (>0) on success, 0 if username already exists, -1 on error
        int create_user(const std::string& username, const std::string& password_hash);

        void delete_expired_sessions();
        // Returns 1 if deleted, 0 if session_id not found, -1 on error
        int delete_session(const std::string& session_id);

        // Returns user_id (>0) if session exists and is not expired, 0 if invalid/expired, -1 on error
        int validate_session(const std::string& session_id);
};
#endif