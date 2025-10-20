#ifndef REGISTERDEVICE_H
#define REGISTERDEVICE_H

#include <httpserver.hpp>
#include <HTTPResources.h>
#include <DBQ.h>

class register_device : public httpserver::http_resource, public HTTPResources {
    public:
        int extract_key_value(nlohmann::json json, std::string key, std::string &value);

        std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);

    private:
        DBQ m_dbq;
};

#endif