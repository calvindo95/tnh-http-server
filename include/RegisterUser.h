#pragma once

#include <httpserver.hpp>
#include <HTTPResources.h>
#include <AuthResources.h>
#include <DBQ.h>

class register_user : public httpserver::http_resource, public HTTPResources, public AuthResources {
    DBQ m_dbq;

    std::string hash_password(const std::string& password);

public:
    std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
};
