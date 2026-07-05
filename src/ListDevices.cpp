#include <ListDevices.h>

#include <iomanip>
#include <sstream>

std::shared_ptr<httpserver::http_response> list_devices::render(const httpserver::http_request& req){
    nlohmann::json data = m_dbq.list_device_count_json();

    if(req.get_header("Content-Type") == "application/json"){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response(data.dump() + "\n", 200, "application/json")
        );
    }

    std::ostringstream ss;
    ss << std::left << std::setw(12) << "DeviceID" << std::setw(18) << "Device Name" << std::setw(12) << "Count" << "\n";
    for(const auto& d : data){
        ss << std::setw(12) << d["DeviceID"].get<int>()
           << std::setw(18) << d["DeviceName"].get<std::string>()
           << std::setw(12) << d["Count"].get<uint64_t>() << "\n";
    }

    return std::shared_ptr<httpserver::http_response>(
        new httpserver::string_response(ss.str())
    );
}