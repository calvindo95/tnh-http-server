#include <httpserver.hpp>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <thread>
#include <filesystem>
#include <atomic>

#include <Config.h>                 // Config
#include <Logging.h>                // Logging
#include <Resource.h>               // Resource

void custom_access_log(const std::string& url){
    // I will probably log something here when clients connect
}

int main(int argc, char** argv) {
    Logging log;
    log.init();

    std::stringstream ss;
    ss << "Initializing tnh-server" << std::endl;
    log.log(Logging::severity_level::normal, ss, "GENTRACE");

    Config& config = Config::get_instance();

    httpserver::create_webserver cw = httpserver::create_webserver(config.GET_HTTP_PORT())
        // set required parameters
        .max_connections(config.GET_MAX_CONNECTIONS())
        .connection_timeout(config.GET_CONNECTION_TIMEOUT())
        .memory_limit(config.GET_MEMORY_LIMIT())
        .start_method(httpserver::http::http_utils::INTERNAL_SELECT)
        .max_threads(config.GET_MAX_THREADS())
        .log_access(custom_access_log)
        .use_dual_stack();

    // create webserver
    httpserver::webserver ws = cw;
    
    // create resource and endpoint
    check_connection hwr;
    ws.register_resource("/helloworld", &hwr);
    
    post_json pj;
    ws.register_resource("/post_json", &pj);

    // start web server
    ws.start(true);

    return 0;
}