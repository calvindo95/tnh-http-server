#include <chrono>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <chrono>

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
        std::string header(headers["Content-Type"]);

        m_logger.log(Logging::severity_level::warning, std::string("Post request Content-Type is not application/json; received header: " + header), "GENTRACE");
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received Content-Type: " + header + "\n"));
    }

    // Get body of request to string
    std::string tmp = std::string(req.get_content());

    // Parse json string into json object
    ret_val += parse_json(tmp,tmp_j);

    if(ret_val == 0){
        m_tsq.push(tmp_j);
        std::stringstream ss;
         //ss << "Inserting data into processing queue: " << tmp_j << std::endl;
        ss << "Processing queue size increased by 1";
        m_logger.log(Logging::severity_level::trace, ss, "QUEUE");
    }
    else{
        std::stringstream ss;
        ss << "Failed insert json into queue: " << tmp << std::endl;
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
    }

    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received data value: " + std::to_string(ret_val) + "\n"));
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
        int queue_size = 0;

        // Wait for first message
        nlohmann::json j = m_tsq.pop(queue_size);

        ssq << "INSERT INTO History (Temperature, Humidity,DeviceID, CurrentDateTime) VALUES(" << j["Temperature"] << "," << j["Humidity"] << ","<< j["DeviceID"] << "," << j["CurrentDateTime"] << ");";

        // If many messages in queue, add multiple inserts to query
        for(int i = 0; i < queue_size; i++){
            std::stringstream ssq_temp;
            nlohmann::json j_temp = m_tsq.pop();

            ssq_temp << "INSERT INTO History (Temperature, Humidity,DeviceID, CurrentDateTime) VALUES(" << j_temp["Temperature"] << "," << j_temp["Humidity"] << ","<< j_temp["DeviceID"] << "," << j_temp["CurrentDateTime"] << ");";


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

        m_logger.log(Logging::severity_level::warning, std::to_string(DeviceID), "GENTRACE");

        std::string output;

        ssq << "SELECT JSON_OBJECT('CurrentDateTime', `CurrentDateTime`, 'DeviceID', `DeviceID`, 'Temperature', `Temperature`, 'Humidity', `Humidity`) FROM History WHERE DeviceID = " << DeviceID << " ORDER BY CurrentDateTime DESC LIMIT 1;";

        if(dbq.select(ssq.str(), output) != 0){
            m_logger.log(Logging::severity_level::warning, "Failed to execute select " + ssq.str(), "GENTRACE");
        }

        m_logger.log(Logging::severity_level::warning, output, "GENTRACE");

        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(output + "\n"));
    }
}