#include <GetHistory.h>

/********************

Class get_history
Input:  {"DeviceID": 1, "StartDateTime": "2026-06-07 00:00:00", "EndDateTime": "2026-06-14 23:59:59", "BucketMinutes": 30}
Return: [{"CurrentDateTime": "...", "Temperature": 68.5, "Humidity": 72.0}, ...]

********************/

std::shared_ptr<httpserver::http_response> get_history::render(const httpserver::http_request& req){
    nlohmann::json req_json;
    nlohmann::json j;

    HTTPResources::get_req_body_json(req, req_json);

    if(req_json.is_null() || req_json.contains("error")){
        j["error"] = "Error parsing request json";
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump(), 400, "application/json"));
    }

    if(!req_json.contains("DeviceID") || !req_json.contains("StartDateTime") || !req_json.contains("EndDateTime")){
        j["error"] = "Missing required fields: DeviceID, StartDateTime, EndDateTime";
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump(), 400, "application/json"));
    }

    int device_id       = req_json["DeviceID"].get<int>();
    std::string start   = req_json["StartDateTime"].get<std::string>();
    std::string end     = req_json["EndDateTime"].get<std::string>();
    int bucket_minutes  = req_json.contains("BucketMinutes") ? req_json["BucketMinutes"].get<int>() : 5;

    nlohmann::json result = m_dbq.get_device_history(device_id, start, end, bucket_minutes);

    return std::shared_ptr<httpserver::http_response>(
        new httpserver::string_response(result.dump() + "\n", 200, "application/json")
    );
}
