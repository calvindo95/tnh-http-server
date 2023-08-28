#pragma once

#include <httpserver.hpp>
#include <thread>

#include <HTTPResources.h>
#include <TSQueue.h>
#include <Logging.h>
#include <nlohmann/json.hpp>

class check_connection : public httpserver::http_resource {
    public:
        std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
};

class post_json : public httpserver::http_resource, public HTTPResources {
    public:
        post_json();

        std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
    private:
        // "DeviceID", "hash", "CurrentDateTime", "Temperature", "Humidity"
        nlohmann::json m_json;

        // Function to consume items in tsqueue
        void consume_thread()noexcept;

        // Parse csv into std::map<std::string,std::string>
        int parse_json(std::string json_string);
};