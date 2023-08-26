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

insert_data::insert_data(){
    // Initialize consuming thread
    std::thread t1(&insert_data::consume_thread, this);
    t1.detach();
}

std::shared_ptr<httpserver::http_response> insert_data::render(const httpserver::http_request& req) {
    int ret_val = 0; 
    std::stringstream ss;
    std::string data = std::string(req.get_arg("data")); // Update data to parse json instead of csv

    ret_val += parse_csv(data);

    ss << "Inserting data into queue: " << data;
    m_logger.log_trace(ss.str(), "GENTRACE");
    //std::cout << "Queue size before insert:" << m_tsq.size() << std::endl;

    m_tsq.push(m_data_map);
    
    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Received data value: " + std::to_string(ret_val)));
}

int insert_data::parse_csv(std::string csv){
    // dev_id,pw,time,temperature,humidity
    std::vector<std::string> temp_vec;
    std::stringstream ss(csv);

    // Break csv into vector
    while(ss.good()){
        std::string substr;
        getline(ss,substr, ',');
        temp_vec.push_back(substr);
    }

    if(temp_vec.size() != 5){
        m_logger.log_trace("Input csv does not have correct indexes", "GENTRACE");
        return 1;
    }

    m_data_map["DeviceID"]          = temp_vec[0];
    m_data_map["hash"]              = temp_vec[1];
    m_data_map["CurrentDateTime"]   = temp_vec[2];
    m_data_map["Temperature"]       = temp_vec[3];
    m_data_map["Humidity"]          = temp_vec[4];

    return 0;
}

void insert_data::consume_thread() noexcept{

    while(true){
        std::stringstream ss;
        std::stringstream ssq1;
        std::stringstream ssq2;
        DBQuery dbq;
 
        std::map<std::string,std::string> temp_map = m_tsq.pop();

        // Build insert queries
        ssq1 << "INSERT INTO History (Temperature, Humidity) VALUES(" << temp_map["Temperature"] << "," << temp_map["Humidity"] << ")";
        ssq2 << "INSERT INTO Data_History (DeviceID, HistoryID, CurrentDateTime) VALUES (" << temp_map["DeviceID"] << ",LAST_INSERT_ID(),'" << temp_map["CurrentDateTime"] << "')";

        dbq.insert(ssq1.str());
        dbq.insert(ssq2.str());

        ss << "Queue size reduced by 1 to: " << m_tsq.size();
        m_logger.log_trace(ss.str(), "GENTRACE");
    }
}