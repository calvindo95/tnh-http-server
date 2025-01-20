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
         //ss << "Inserting data into processing queue: " << tmp_j << std::endl;
        ss << "Processing queue size increased by 1"G;
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
    std::stringstream ss;
    while(true){
        DBQuery dbq;
        int ret_val = 0;
        std::stringstream ssq;

        // Wait for first message
        nlohmann::json j = m_tsq.pop();

        ssq << "INSERT INTO History (Temperature, Humidity) VALUES(" << j["Temperature"] << "," << j["Humidity"] << "); \
        INSERT INTO Data_History (DeviceID, HistoryID, CurrentDateTime) VALUES (" << j["DeviceID"] << ",LAST_INSERT_ID()," << j["CurrentDateTime"] << ");";

        // If many messages in queue, add multiple inserts to query
        int queue_size = m_tsq.size();
        for(int i = 0; i < queue_size; i++){
            std::stringstream ssq_temp;
            nlohmann::json j_temp = m_tsq.pop();

            ssq_temp << "INSERT INTO History (Temperature, Humidity) VALUES(" << j_temp["Temperature"] << "," << j_temp["Humidity"] << "); \
            INSERT INTO Data_History (DeviceID, HistoryID, CurrentDateTime) VALUES (" << j_temp["DeviceID"] << ",LAST_INSERT_ID()," << j_temp["CurrentDateTime"] << ");";

            ssq << ssq_temp.str();
        }

        // Execute query
        ret_val += dbq.insert(ssq.str());

        if(ret_val != 0){
            ss << "Error inserting json data: " << ssq.str();
            m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
            ss.str(std::string());
            ss.clear();
        }
        else{
            ss << "Processing queue size reduced by " << queue_size+1;
            m_logger.log(Logging::severity_level::trace, ss, "QUEUE");
            ss.str(std::string());
            ss.clear();
        }
    }
}