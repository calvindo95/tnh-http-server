#include "Config.h"

Config::Config(bool debug): 
    DEBUG_FLAG(debug),
    HTTP_PORT(8080), 
    MAX_CONNECTIONS(32), 
    CONNECTION_TIMEOUT(180), 
    MEMORY_LIMIT(32768),
    MAX_THREADS(1), 
    HTTPS_MEM_KEY("./certs/server_ca/private/smoothstack_server.key"),
    HTTPS_MEM_CERT("./certs/server_ca/certs/smoothstack_server.crt"),
    DB_IP("127.0.0.1"),
    DB_USERNAME("admin"),
    DB_PASSWORD("password"),
    DB_NAME(""),
    DB_PORT(9090){
    // update values if environment variables are set
    update_config();

    if(DEBUG_FLAG){
        std::cout << "Server option HTTP_Port:         "<< GET_HTTP_PORT() << std::endl;
        std::cout << "Server option MAX_CONNECTIONS:   "<< GET_MAX_CONNECTIONS() << std::endl;
        std::cout << "Server option CONNECTION_TIMEOUT:"<< GET_CONNECTION_TIMEOUT() << std::endl;
        std::cout << "Server option MEMORY_LIMIT:      "<< GET_MEMORY_LIMIT() << std::endl;
        std::cout << "Server option MAX_THREADS:       "<< GET_MAX_THREADS() << std::endl;
    }
}

template <typename T>
void Config::update_option(T& option, const char* env_var){
    // Check settings.json
    std::ifstream ifs;
    ifs.open("./settings.json");

    json j = json::parse(ifs);
    std::stringstream ss;

    // Check for env var
    char* buffer = getenv(env_var);
    if(buffer != NULL){
        option = static_cast<T>(getenv(env_var));

        ss << "Config: " << env_var << " is set as an env variable" << std::endl;
        m_logger.log(Logging::severity_level::normal, ss, "GENTRACE");
    }
    // Check json if env var doesn't exist
    else{
        ss << "Config: " << env_var << " is not set as an env variable, checking settings.json" << std::endl;
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
        ss.str(std::string());
        ss.clear();


        if(j.contains(env_var)){
            ss << "Config: " << env_var << " found in settings.json" << std::endl;
            m_logger.log(Logging::severity_level::normal, ss, "GENTRACE");
            option = static_cast<T>(j.at(env_var));
        }
        else{
            ss << "Config: " << env_var << " not found in settings.json" << std::endl;
            m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
        }
    }
}

template <typename T, unsigned int base>
void Config::update_option(T& option, const char* env_var){
    // Check settings.json
    std::ifstream ifs;
    ifs.open("./settings.json");

    json j = json::parse(ifs);
    std::stringstream ss;

    // Check env variables for settings; ENV vars take priority
    char* buffer = getenv(env_var);
    try{
        if(buffer != NULL){
            int env_num = std::stoi(buffer);
            if(env_num > 0 && env_num < 65536){
                option = static_cast<T>(std::stoul(buffer, nullptr, base));
                
                ss << "Config: " << env_var << " is set as an env variable" << std::endl;
                m_logger.log(Logging::severity_level::normal, ss, "GENTRACE");
            }
            else{
                throw(buffer);
            }
        }
        else{
            ss << "Config: " << env_var << " is not set as an env variable, checking settings.json" << std::endl;
            m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
            ss.str(std::string());
            ss.clear();

            if(j.contains(env_var)){
                ss << "Config: " << env_var << " found in settings.json" << std::endl;
                m_logger.log(Logging::severity_level::normal, ss, "GENTRACE");

                std::string tmp_str = nlohmann::to_string(j.at(env_var));
                tmp_str.erase(std::remove(tmp_str.begin(), tmp_str.end(), '"'), tmp_str.end());

                const char* buf = tmp_str.c_str();
                option = static_cast<T>(std::stoul(buf, nullptr, base));
            }
            else{
                ss << "Config: " << env_var << " is not set in settings.json, checking Env variables" << std::endl;
                m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
            }

            option = option;
        }
    }
    catch(const char* e){
        ss << "ERROR: " << env_var << " - " << e << " out of bounds" << std::endl;
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
        exit(1);
    }
}

Config& Config::get_instance(bool debug){
    static Config config(debug);
    return config;
}
// #### PUBLIC METHODS START HERE #### //

// this should only ever be used for testing purposes outside of the class
void Config::update_config(){
    update_option<uint16_t, 10>   (HTTP_PORT, "HTTP_PORT");
    update_option<std::string>    (HTTPS_MEM_KEY, "HTTPS_MEM_KEY_PATH");
    update_option<std::string>    (HTTPS_MEM_CERT, "HTTPS_MEM_CERT_PATH");
    update_option<uint16_t, 10>   (MAX_CONNECTIONS, "MAX_CONNECTIONS");
    update_option<uint16_t, 10>   (CONNECTION_TIMEOUT, "CONNECTION_TIMEOUT");
    update_option<uint16_t, 10>   (MEMORY_LIMIT, "MEMORY_LIMIT");
    update_option<uint16_t, 10>   (MAX_THREADS, "MAX_THREADS");
    update_option<std::string>    (DB_IP, "DB_IP");
    update_option<std::string>    (DB_USERNAME, "DB_USERNAME");
    update_option<std::string>    (DB_PASSWORD, "DB_PASSWORD");
    update_option<std::string>    (DB_NAME, "DB_NAME");
    update_option<uint16_t, 10>   (DB_PORT, "DB_PORT");
}

// getters
uint16_t Config::GET_HTTP_PORT(){
    return HTTP_PORT;
}
uint16_t Config::GET_MAX_CONNECTIONS(){
    return MAX_CONNECTIONS;
}
uint16_t Config::GET_CONNECTION_TIMEOUT(){
    return CONNECTION_TIMEOUT;
}
uint16_t Config::GET_MEMORY_LIMIT(){
    return MEMORY_LIMIT;
}
uint16_t Config::GET_MAX_THREADS(){
    // if more than 4 cores is set, start a thread pool (>1 cores automatically starts a thread pool)
    const int processor_count = std::thread::hardware_concurrency();
    if(MAX_THREADS >= 4 && processor_count >= 4){
        return MAX_THREADS;
    }
    return 1;
}

std::string Config::GET_DB_IP(){
    return DB_IP;
}
std::string Config::GET_DB_USERNAME(){
    return DB_USERNAME;
}
std::string Config::GET_DB_PASSWORD(){
    return DB_PASSWORD;
}
std::string Config::GET_DB_NAME(){
    return DB_NAME;
}
uint16_t    Config::GET_DB_PORT(){
    return DB_PORT;
}