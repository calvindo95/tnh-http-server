#include <GetLastEntry.h>

/********************
 
Class get_last_entry
Input:  {"DeviceID": 1}
Return: {"CurrentDateTime":"2026-06-14 17:40:01","DeviceID":1}

********************/

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

    // Query DB and get last entry for DeviceID
    m_dbq.get_last_device_entry(req_json["DeviceID"], j);

    // if j is null or contains error key
    if(j.contains("error")){
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump()));
    }
    
    // Return CurrentDateTime and DevID in json format
    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump()));
}