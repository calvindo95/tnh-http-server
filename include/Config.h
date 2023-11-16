#include <string>
#include <json.hpp>
#include <fstream>
#include <algorithm>
#include <thread>
#include <iostream>
#include <cstring>

#include <Logging.h>

using json = nlohmann::json;

class Config{
    private:
        Config(bool debug = true);
        bool             DEBUG_FLAG;
        Logging m_logger;

        // Server Configs
        uint16_t         HTTP_PORT;
        uint16_t         MAX_CONNECTIONS;
        uint16_t         CONNECTION_TIMEOUT;
        uint16_t         MEMORY_LIMIT;
        uint16_t         MAX_THREADS;
        std::string      HTTPS_MEM_KEY;
        std::string      HTTPS_MEM_CERT;

        // HTTP Server Configs
        std::string      DB_IP;
        std::string      DB_USERNAME;
        std::string      DB_PASSWORD;
        std::string      DB_NAME;
        uint16_t         DB_PORT;

        template <typename T>
        void update_option(T& option, const char* env_var);

        template <typename T, unsigned int base>
        void update_option(T& option, const char* env_var);

    public:
        static Config& get_instance(bool debug = true);

        void update_config();

        uint16_t         GET_HTTP_PORT();
        uint16_t         GET_MAX_CONNECTIONS();
        uint16_t         GET_CONNECTION_TIMEOUT();
        uint16_t         GET_MEMORY_LIMIT();
        uint16_t         GET_MAX_THREADS();

        std::string      GET_DB_IP();
        std::string      GET_DB_USERNAME();
        std::string      GET_DB_PASSWORD();
        std::string      GET_DB_NAME();
        uint16_t         GET_DB_PORT();
};