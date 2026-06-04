#include <chrono>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <unistd.h>

#include <Resource.h>
#include <DBQuery.h>

std::shared_ptr<httpserver::http_response> not_found_custom(const httpserver::http_request& req) {
    return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Not found custom", 404, "text/plain"));
}

// this has not been fully implemented
std::shared_ptr<httpserver::http_response> not_allowed_custom(const httpserver::http_request& req) {
    return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Not allowed custom", 405, "text/plain"));
}

std::shared_ptr<httpserver::http_response> check_connection::render(const httpserver::http_request& req){
    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Hello, World!"));
}


/********************
 
Class get_single_data

********************/

std::shared_ptr<httpserver::http_response> get_single_data::render(const httpserver::http_request& req){
    int ret_val = 0; 
    nlohmann::json tmp_j;

    DBQuery dbq;
    
    std::map<std::string_view, std::string_view, httpserver::http::header_comparator> headers;
    headers = req.get_headers();

    if(headers["Content-Type"] != "application/json"){
        std::string header(headers["Content-Type"]);

        m_logger.log(Logging::severity_level::warning, std::string("Post request Content-Type is not application/json; received header: " + header), "GENTRACE");
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received Content-Type: " + header + "\n"));
    }

    // Get body of request to string
    std::string tmp = std::string(req.get_content());

    // Parse json string into json object
    tmp_j = nlohmann::json::parse(tmp);

    if(ret_val != 0){
        std::stringstream ss;
        ss << "Failed to parse json in get_single_data::render() " << tmp << std::endl;
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");

        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received data value: " + std::to_string(ret_val=1) + "\n"));
    }
    else{
        std::stringstream ssq;
        int DeviceID = tmp_j["DeviceID"];

        std::string output;

        ssq << "SELECT JSON_OBJECT('CurrentDateTime', `CurrentDateTime`, 'DeviceID', `DeviceID`, 'Temperature', `Temperature`, 'Humidity', `Humidity`) FROM History WHERE DeviceID = " << DeviceID << " ORDER BY CurrentDateTime DESC LIMIT 1;";

        if(dbq.select(ssq.str(), output) != 0){
            m_logger.log(Logging::severity_level::warning, "Failed to execute select " + ssq.str(), "GENTRACE");
        }

        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(output + "\n"));
    }
}