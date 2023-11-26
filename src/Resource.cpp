#include <chrono>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <chrono>

#include <Resource.h>
#include <DBQuery.h>

std::shared_ptr<httpserver::http_response> check_connection::render(const httpserver::http_request& req){
    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Hello, World!"));
}

/********************
 
Class post_json

********************/
post_json::post_json(){
    // Initialize consuming thread
    std::thread t1(&post_json::consume_thread, this);
    t1.detach();
}

std::shared_ptr<httpserver::http_response> post_json::render(const httpserver::http_request& req) {
    int ret_val = 0; 
    nlohmann::json tmp_j;
    
    std::map<std::string_view, std::string_view, httpserver::http::header_comparator> headers;
    headers = req.get_headers();

    if(headers["Content-Type"] != "application/json"){
        m_logger.log(Logging::severity_level::warning, std::string("Post request Content-Type is not application/json"), "GENTRACE");
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received data value: " + std::to_string(ret_val=1)));
    }

    // Get body of request to string
    std::string tmp = std::string(req.get_content());

    // Parse json string into json object
    ret_val += parse_json(tmp,tmp_j);

    if(ret_val == 0){
        m_tsq.push(tmp_j);
        std::stringstream ss;
        ss << "Inserting data into queue: " << tmp_j << std::endl;
        m_logger.log(Logging::severity_level::trace, ss, "QUEUE");
    }
    else{
        std::stringstream ss;
        ss << "Failed insert json into queue: " << tmp << std::endl;
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
    }

    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received data value: " + std::to_string(ret_val)));
}

int post_json::parse_json(std::string json_string, nlohmann::json& json){
    std::stringstream ss;

    if(!nlohmann::json::accept(json_string)){
        ss << "Failed to parse json string: " << json_string << std::endl;
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
        return 1;
    }

    json = nlohmann::json::parse(json_string);
    return 0;
}

void post_json::consume_thread() noexcept{
    while(true){
        int ret_val = 0;
        std::stringstream ss;
 
        nlohmann::json j = m_tsq.pop();

        // DeviceID hash CurrentDateTime Temperature Humidity
        if(j.contains("DeviceID") && j.contains("hash") && j.contains("CurrentDateTime") && j.contains("Temperature") && j.contains("Humidity")){
            DBQuery dbq;
            std::stringstream ssq;
            // Build insert query
            ssq << "INSERT INTO History (Temperature, Humidity) VALUES(" << j["Temperature"] << "," << j["Humidity"] << "); \
            INSERT INTO Data_History (DeviceID, HistoryID, CurrentDateTime) VALUES (" << j["DeviceID"] << ",LAST_INSERT_ID()," << j["CurrentDateTime"] << ");";

            ret_val += dbq.insert(ssq.str());

            if(ret_val != 0){
                ss << "Error inserting json data: " << j;
                m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
                ss.str(std::string());
                ss.clear();
            }
        }

        // Application Code PID Text Time UID
        if(j.contains("Application") && j.contains("Dev_ID") && j.contains("Text") && j.contains("UID")){
            ss << j.dump() << std::endl;
            m_logger.log(Logging::severity_level::trace, ss, "EVENT");
            ss.str(std::string());
            ss.clear();
        }

        ss << "Queue size reduced by 1 to: " << m_tsq.size();
        m_logger.log(Logging::severity_level::trace, ss, "QUEUE");
    }
}