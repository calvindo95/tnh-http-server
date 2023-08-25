#pragma once

#include <httpserver.hpp>
#include <thread>

#include <HTTPResources.h>
#include <TSQueue.h>
#include <Logging.h>

class check_connection : public httpserver::http_resource {
    public:
        std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
};

class insert_data : public httpserver::http_resource, public HTTPResources {
    public:
        insert_data();

        std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
    private:
        // "dev_id","hash","time","temp","humidity"
        std::map<std::string,std::string> m_data_map;

        // Function to consume items in tsqueue
        void consume_thread()noexcept;

        // Parse csv into std::map<std::string,std::string>
        int parse_csv(std::string csv);
};