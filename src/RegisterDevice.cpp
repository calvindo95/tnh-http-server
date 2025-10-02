#include <RegisterDevice.h>

#include <sstream>

#include <DBQuery.h>
#include <Logging.h>

#include <json.hpp>

int register_device::extract_key_value(nlohmann::json json, std::string key, std::string &value){
    if(!json.contains(key)){
        std::stringstream ss;
        ss << "JSON does not contain key '" << key << "': " << json.dump() << std::endl;
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");

        return 1;
    }

    value = json[key];

    return 0;
}

int register_device::parse_json(std::string json_string, nlohmann::json& json){
    std::stringstream ss;

    if(!nlohmann::json::accept(json_string)){
        ss << "Failed to parse json string: " << json_string << std::endl;
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");
        return 1;
    }

    json = nlohmann::json::parse(json_string);
    return 0;
}

int register_device::get_body_json(const httpserver::http_request &req, nlohmann::json &json){
    std::map<std::string_view, std::string_view, httpserver::http::header_comparator> headers;
    headers = req.get_headers();

    if(headers["Content-Type"] != "application/json"){
        std::string header(headers["Content-Type"]);

        m_logger.log(Logging::severity_level::warning, std::string("Post request Content-Type is not application/json; received header: " + header), "GENTRACE");
        return 1;
    }

    std::string body_string = std::string(req.get_content());

    if(parse_json(body_string, json) != 0){
        return 1;
    }

    return 0;
}

std::shared_ptr<httpserver::http_response> register_device::render(const httpserver::http_request& req){
    nlohmann::json body_json;
    int ret =  0;
    
    // get device name
    if(get_body_json(req, body_json) != 0){
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(body_json.dump()));
    }

    std::string device_name;

    if(extract_key_value(body_json, "DevName", device_name) != 0){
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(body_json.dump()));
    }

    // check if device name exists in dev_name table
    ret = m_dbq.get_device_id(device_name);
    if(ret == -1){
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Error checking if DevName exists: " + body_json.dump()));
    }
    else if(ret == 0){
        int devid;

        m_logger.log(Logging::severity_level::info, std::string("DevName " + device_name + " does not exist, inserting into DB"), "GENTRACE");
        
        m_dbq.insert_devname(device_name);
        devid = m_dbq.get_device_id(device_name);

        if(devid <= 0){
            return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Failed to insert DevName " + device_name + " into DB\n"));
        }

        m_logger.log(Logging::severity_level::info, std::string("Successfully inserted DevName " + device_name + " into DB"), "GENTRACE");

        nlohmann::json j;

        j["DeviceID"] = devid;
        j["DevName"] = device_name;

        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump()));
    }
    else{
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("DevName " + device_name + " exists with DevID of: " + std::to_string(ret) + "\n"));
    }
    
    // if name exists return existing num

    // if name does not exist, insert dev_name and return new num


    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(device_name + "\n"));
}