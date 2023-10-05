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
    std::stringstream ss;
    nlohmann::json tmp_j;
    
    std::map<std::string_view, std::string_view, httpserver::http::header_comparator> headers;
    headers = req.get_headers();

    if(headers["Content-Type"] != "application/json"){
        m_logger.log_trace(std::string("Post request Content-Type is not application/json"), "GENTRACE");
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received data value: " + std::to_string(ret_val=1)));
    }

    // Get body of request to string
    std::string tmp = std::string(req.get_content());

    // Parse json string into json object
    ret_val += parse_json(tmp,tmp_j);

    if(ret_val == 0){
        ss << "Inserting data into queue: " << tmp_j;
        m_logger.log_trace(ss.str(), "GENTRACE");
        m_tsq.push(tmp_j);
    }

    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received data value: " + std::to_string(ret_val)));
}

int post_json::parse_json(std::string json_string, nlohmann::json& json){
    std::stringstream ss;
    json = nlohmann::json::parse(json_string);

    // Check if json has correct number of key/values
    if(json.size() != 5){
        m_logger.log_trace("Input json does not have correct size(5)", "GENTRACE");
        return 1;
    }

    // Check if correct keys
    for(int i = 0; i < m_json_keys.size(); i++){
        if(json.count(m_json_keys[i]) != 1){
            ss << "JSON key value: '" << m_json_keys[i] << "' is not present in post json";
            m_logger.log_trace(ss.str(), "GENTRACE");
            return 1;
        }
    }
    return 0;
}

void post_json::consume_thread() noexcept{
    while(true){
        int ret_val = 0;
        std::stringstream ss;
        std::stringstream ssq;
        DBQuery dbq;
 
        nlohmann::json j = m_tsq.pop();

        // Build insert query
        ssq << "INSERT INTO History (Temperature, Humidity) VALUES(" << j["Temperature"] << "," << j["Humidity"] << "); \
                INSERT INTO Data_History (DeviceID, HistoryID, CurrentDateTime) VALUES (" << j["DeviceID"] << ",LAST_INSERT_ID()," << j["CurrentDateTime"] << ");";

        ret_val += dbq.insert(ssq.str());

        if(ret_val != 0){
            ss << "Error inserting json data: " << j;
            m_logger.log_trace(ss.str(), "GENTRACE");
            ss.clear();
            ss.str(std::string());
        }

        ss << "Queue size reduced by 1 to: " << m_tsq.size();
        m_logger.log_trace(ss.str(), "GENTRACE");
    }
}