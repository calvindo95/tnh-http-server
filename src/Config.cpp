#include "Config.h"
#include <thread>
#include <iostream>
#include <cstring>

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
T Config::update_option(T& option, const char* env_var){
    char* buffer = getenv(env_var);
    if(buffer != NULL){
        return static_cast<T>(getenv(env_var));
    }
    else{
        return option;
    }
}

template <typename T, unsigned int base>
T Config::update_option(T& option, const char* env_var){
    char* buffer = getenv(env_var);
    try{
        if(buffer != NULL){
            int env_num = std::stoi(buffer);
            if(env_num > 0 && env_num < 65536){
                return static_cast<T>(std::stoul(buffer, nullptr, base));
            }
            else{
                throw(buffer);
            }
        }
        else{
            return option;
        }
    }
    catch(const char* e){
        std::cout << "ERROR: " << env_var << " - " << e << " out of bounds" << std::endl;
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
    HTTP_PORT           = update_option<uint16_t, 10>   (HTTP_PORT, "HTTP_PORT");
    HTTPS_MEM_KEY       = update_option<std::string>    (HTTPS_MEM_KEY, "HTTPS_MEM_KEY_PATH");
    HTTPS_MEM_CERT      = update_option<std::string>    (HTTPS_MEM_CERT, "HTTPS_MEM_CERT_PATH");
    MAX_CONNECTIONS     = update_option<uint16_t, 10>   (MAX_CONNECTIONS, "MAX_CONNECTIONS");
    CONNECTION_TIMEOUT  = update_option<uint16_t, 10>   (CONNECTION_TIMEOUT, "CONNECTION_TIMEOUT");
    MEMORY_LIMIT        = update_option<uint16_t, 10>   (MEMORY_LIMIT, "MEMORY_LIMIT");
    MAX_THREADS         = update_option<uint16_t, 10>   (MAX_THREADS, "MAX_THREADS");
    DB_IP               = update_option<std::string>    (DB_IP, "DB_IP");
    DB_USERNAME         = update_option<std::string>    (DB_USERNAME, "DB_USERNAME");
    DB_PASSWORD         = update_option<std::string>    (DB_PASSWORD, "DB_PASSWORD");
    DB_NAME             = update_option<std::string>    (DB_NAME, "DB_NAME");
    DB_PORT             = update_option<uint16_t, 10>   (DB_PORT, "DB_PORT");
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