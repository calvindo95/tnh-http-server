#pragma once

#include <json.hpp>
#include <httpserver.hpp>

#include <TSQueue.h>
#include <Logging.h>
#include <DBQ.h>

class HTTPResources{
    protected:
        Logging m_logger;
        TSQueue<nlohmann::json> m_tsq;

        void parse_json(std::string json_string, nlohmann::json &json);
        void get_req_body_json(const httpserver::http_request &req, nlohmann::json &json);
        std::string get_bearer_token(const httpserver::http_request &req);
};