#pragma once

#include <httpserver.hpp>
#include <thread>

#include <HTTPResources.h>
#include <TSQueue.h>
#include <Logging.h>
#include <json.hpp>

std::shared_ptr<httpserver::http_response> not_found_custom(const httpserver::http_request& req);

std::shared_ptr<httpserver::http_response> not_allowed_custom(const httpserver::http_request& req);

class check_connection : public httpserver::http_resource {
    public:
        std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
};