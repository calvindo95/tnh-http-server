#pragma once

#include <thread>

#include <httpserver.hpp>
#include <HTTPResources.h>
#include <DBQ.h>

class auth_serv : public httpserver::http_resource, public HTTPResources {
    DBQ m_dbq;

    void cleanup_thread() noexcept;

public:
    auth_serv();
    std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
};
