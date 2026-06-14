#pragma once

#include <httpserver.hpp>
#include <HTTPResources.h>
#include <DBQ.h>
#include <Logging.h>
#include <json.hpp>

class get_history : public httpserver::http_resource, public HTTPResources {
    Logging m_logger;
public:
    std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
};
