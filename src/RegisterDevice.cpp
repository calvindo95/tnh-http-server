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

// Endpoint expects json {"DevName": "Hallway"}
// Returns json with DeviceID {"DevName":"Hallway","DeviceID":7}
std::shared_ptr<httpserver::http_response> register_device::render(const httpserver::http_request& req){
    nlohmann::json body_json;
    std::string device_name;
    int ret =  0;
    
    // get device name
    HTTPResources::get_req_body_json(req, body_json);

    // Make sure json body has no errors
    if(body_json.is_null()){
        body_json["error"] = "Error parsing request json";
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(body_json.dump()));
    }
    if(body_json.contains("error")){
        body_json["error"] = "Error parsing request json; json object does not contain DevName key";
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(body_json.dump()));
    }

    // Extract value from key DevName
    if(extract_key_value(body_json, "DevName", device_name) != 0){
        body_json["error"] = "Json object does not contain key DevName";
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(body_json.dump()));
    }

    // check if device name exists in dev_name table
    ret = m_dbq.get_device_id(device_name);
    if(ret == -1){
        body_json["error"] = "Error checking if DevName exists";
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(body_json.dump()));
    }
    // Insert devname if it doesn't exist in DB
    // Returns devname and new DevID
    else if(ret == 0){
        int devid;

        m_logger.log(Logging::severity_level::info, std::string("DevName " + device_name + " does not exist, inserting into DB"), "GENTRACE");
        
        m_dbq.insert_devname(device_name);
        devid = m_dbq.get_device_id(device_name);

        if(devid <= 0){
            std::stringstream ss;
            ss << "Failed to insert DevName " << device_name << " into DB\n";

            body_json["error"] = ss.str();
            return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(body_json.dump()));
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