#ifndef REGISTERDEVICE_H
#define REGISTERDEVICE_H

#include <httpserver.hpp>
#include <HTTPResources.h>

class register_device : public httpserver::http_resource, public HTTPResources {
    public:
        // Get DevName
        int get_body_json(const httpserver::http_request &req, nlohmann::json &json);
        int parse_json(std::string json_string, nlohmann::json& json);
        int extract_key_value(nlohmann::json json, std::string key, std::string &value);

        // Check if DevName exists in Device table
        int check_if_devname_exists(std::string devname);

        std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
};

#endif