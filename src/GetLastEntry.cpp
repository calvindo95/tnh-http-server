#include <GetLastEntry.h>

std::shared_ptr<httpserver::http_response> get_last_entry::render(const httpserver::http_request& req){
    nlohmann::json req_json;
    nlohmann::json j;

    HTTPResources::get_req_body_json(req, req_json);

    if(req_json.is_null()){
        j["error"] = "Error parsing request json";

        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump()));
    }
    if(req_json.contains("error")){
        j["error"] = "Error parsing request json; json object does not contain DeviceID key";

        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump()));
    }

    m_dbq.get_last_device_entry(req_json["DeviceID"], j);

    // if j is null or contains error key
    if(j.contains("error")){
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump()));
    }
    
    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump()));
}