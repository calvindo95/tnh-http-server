#include <GetSingleData.h>

// Input: {"DeviceID": 1}
// Output {"CurrentDateTime": "2026-06-14 20:19:01", "DeviceID": 1, "Humidity": 77.8127, "Temperature": 68.39}

std::shared_ptr<httpserver::http_response> get_single_data::render(const httpserver::http_request& req){
    nlohmann::json tmp_j;

    auto headers = req.get_headers();

    if(headers["Content-Type"] != "application/json"){
        std::string header(headers["Content-Type"]);
        m_logger.log(Logging::severity_level::warning,
            "Post request Content-Type is not application/json; received header: " + header, "GENTRACE");
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received Content-Type: " + header + "\n"));
    }

    std::string tmp = std::string(req.get_content());

    if(!nlohmann::json::accept(tmp)){
        m_logger.log(Logging::severity_level::warning,
            "Failed to parse json in get_single_data::render(): " + tmp, "GENTRACE");
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Failed to parse JSON\n"));
    }

    tmp_j = nlohmann::json::parse(tmp);

    if(!tmp_j.contains("DeviceID")){
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("{\"error\":\"Missing DeviceID\"}\n", 400, "application/json"));
    }

    int DeviceID = tmp_j["DeviceID"];
    nlohmann::json result;

    m_dbq.get_last_device_data(DeviceID, result);

    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(result.dump(2) + "\n", 200, "application/json"));
}
