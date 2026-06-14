#include <ListDevices.h>

std::shared_ptr<httpserver::http_response> list_devices::render(const httpserver::http_request& req){
    auto headers = req.get_headers();

    if(headers["Content-Type"] == "application/json"){
        nlohmann::json j = m_dbq.list_device_count_json();
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response(j.dump(2) + "\n", 200, "application/json")
        );
    }

    return std::shared_ptr<httpserver::http_response>(
        new httpserver::string_response(m_dbq.list_device_count() + "\n")
    );
}