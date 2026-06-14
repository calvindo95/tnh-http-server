#pragma once

#include <httpserver.hpp>
#include <HTTPResources.h>
#include <DBQ.h>

class list_devices : public httpserver::http_resource, public HTTPResources {
    public:
        std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);

    private:
        DBQ m_dbq;
};