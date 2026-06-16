#pragma once

#include <httpserver.hpp>
#include <HTTPResources.h>
#include <DBQ.h>

class auth_session : public httpserver::http_resource, public HTTPResources {
    DBQ m_dbq;

public:
    std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
};
