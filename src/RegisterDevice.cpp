#include <RegisterDevice.h>

#include <sstream>

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

// Endpoint expects json {"DevName": "Hallway"}
// Returns json with DeviceID {"DevName":"Hallway","DeviceID":7}
std::shared_ptr<httpserver::http_response> register_device::render(const httpserver::http_request& req){
    nlohmann::json body_json;
    std::string device_name;
    int ret =  0;
    
    // get device name
    HTTPResources::get_req_body_json(req, body_json);

    // Make sure json body has no errors
    if(body_json.is_null() || body_json.contains("error")){
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("{\"error\":\"Error parsing request json\"}", 400, "application/json"));
    }

    // Extract value from key DevName
    if(extract_key_value(body_json, "DevName", device_name) != 0){
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("{\"error\":\"Missing DevName\"}", 400, "application/json"));
    }

    if(device_name.empty() || device_name.length() > 64){
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("{\"error\":\"DevName must be between 1 and 64 characters\"}", 400, "application/json"));
    }

    // check if device name exists in dev_name table
    ret = m_dbq.get_device_id(device_name);
    if(ret == -1){
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("{\"error\":\"Internal server error\"}", 500, "application/json"));
    }
    // Insert devname if it doesn't exist in DB
    // Returns devname and new DevID
    else if(ret == 0){
        int devid;

        m_logger.log(Logging::severity_level::info, std::string("DevName " + device_name + " does not exist, inserting into DB"), "GENTRACE");

        m_dbq.insert_devname(device_name);
        devid = m_dbq.get_device_id(device_name);

        if(devid <= 0){
            m_logger.log(Logging::severity_level::warning, "Failed to insert DevName into DB", "GENTRACE");
            return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("{\"error\":\"Internal server error\"}", 500, "application/json"));
        }

        m_logger.log(Logging::severity_level::info, std::string("Successfully inserted DevName " + device_name + " into DB"), "GENTRACE");

        nlohmann::json j;

        j["DeviceID"] = devid;
        j["DevName"] = device_name;

        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump()));
    }
    // If devname already exists, return devname and devid
    else{
        nlohmann::json j;

        j["DeviceID"] = ret;
        j["DevName"] = device_name;

        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(j.dump()));
    }
}