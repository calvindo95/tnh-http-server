#pragma once

#include <httpserver.hpp>
#include <HTTPResources.h>
#include <AuthResources.h>
#include <DBQ.h>

class auth_user : public httpserver::http_resource, public HTTPResources, public AuthResources {
    DBQ m_dbq;

    bool verify_password(const std::string& password, const std::string& stored_hash);
    std::string generate_session_id();

public:
    std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
};
