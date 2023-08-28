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

    std::map<std::string_view, std::string_view, httpserver::http::header_comparator> headers;
    headers = req.get_headers();

    if(headers["Content-Type"] != "application/json"){
        m_logger.log_trace(std::string("Post request Content-Type is not application/json"), "GENTRACE");
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received data value: " + std::to_string(ret_val=1) + "\n"));
    }

    // Get body of request to string
    std::string tmp = std::string(req.get_content());

    // Parse json string into json object
    ret_val += parse_json(tmp);

    if(ret_val == 0){
        ss << "Inserting data into queue: " << tmp;
        m_logger.log_trace(ss.str(), "GENTRACE");
        m_tsq.push(m_json);
    }

    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received data value: " + std::to_string(ret_val) + "\n"));
}

int post_json::parse_json(std::string json_string){
    std::stringstream ss;
    m_json = nlohmann::json::parse(json_string);

    // Check if json has correct number of key/values
    if(m_json.size() != 5){
        m_logger.log_trace("Input json does not have correct size(5)", "GENTRACE");
        return 1;
    }

    // Check if correct keys
    std::vector<std::string> tmp_vec{"DeviceID", "hash", "CurrentDateTime", "Temperature", "Humidity"};
    for(int i = 0; i < tmp_vec.size(); i++){
        if(m_json.count(tmp_vec[i]) != 1){
            ss << "JSON key value: '" << tmp_vec[i] << "' is not present in post json";
            m_logger.log_trace(ss.str(), "GENTRACE");
            return 1;
        }
    }
    return 0;
}

void post_json::consume_thread() noexcept{
    while(true){
        std::stringstream ss;
        std::stringstream ssq1;
        std::stringstream ssq2;
        DBQuery dbq;
 
        nlohmann::json j = m_tsq.pop();

        // Build insert queries
        ssq1 << "INSERT INTO History (Temperature, Humidity) VALUES(" << j["Temperature"] << "," << j["Humidity"] << ")";
        ssq2 << "INSERT INTO Data_History (DeviceID, HistoryID, CurrentDateTime) VALUES (" << j["DeviceID"] << ",LAST_INSERT_ID()," << j["CurrentDateTime"] << ")";

        dbq.insert(ssq1.str());
        dbq.insert(ssq2.str());

        ss << "Queue size reduced by 1 to: " << m_tsq.size();
        m_logger.log_trace(ss.str(), "GENTRACE");
    }
}