/********************
 
Class post_json

********************/

#include <chrono>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <unistd.h>

#include <Post_Json.h>

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
    DBQ dbq;
    while(true){
        int queue_size = 0;
        int processed = 0;

        auto insert_entry = [&](const nlohmann::json& j) {
            if (!j["Temperature"].is_string() || !j["Humidity"].is_string() ||
                !j["DeviceID"].is_string() || !j["CurrentDateTime"].is_string()) {
                m_logger.log(Logging::severity_level::warning, std::string("Invalid field types in json entry"), "GENTRACE");
                return;
            }

            double t, h;
            int did;
            std::string cdt;
            try {
                t   = std::stod(j["Temperature"].get<std::string>());
                h   = std::stod(j["Humidity"].get<std::string>());
                did = std::stoi(j["DeviceID"].get<std::string>());
                cdt = j["CurrentDateTime"].get<std::string>();
            }
            catch (const std::exception& e) {
                m_logger.log(Logging::severity_level::warning,
                    std::string("Failed to parse json fields: ") + e.what(), "GENTRACE");
                return;
            }

            dbq.insert_history(t, h, did, cdt);
            ++processed;
        };

        insert_entry(m_tsq.pop(queue_size));

        for(int i = 0; i < queue_size; i++){
            insert_entry(m_tsq.pop());
        }

        std::stringstream ss;
        ss << "Processing queue size reduced by " << processed;
        m_logger.log(Logging::severity_level::trace, ss, "QUEUE");
    }
}