#include <chrono>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <unistd.h>

#include <Resource.h>

std::shared_ptr<httpserver::http_response> not_found_custom(const httpserver::http_request& req) {
    return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Not found custom", 404, "text/plain"));
}

// this has not been fully implemented
std::shared_ptr<httpserver::http_response> not_allowed_custom(const httpserver::http_request& req) {
    return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Not allowed custom", 405, "text/plain"));
}

std::shared_ptr<httpserver::http_response> check_connection::render(const httpserver::http_request& req){
    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Hello, World!"));
}
