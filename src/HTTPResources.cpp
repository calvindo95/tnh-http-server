#include <HTTPResources.h>

void HTTPResources::parse_json(std::string json_string, nlohmann::json& json){
    std::stringstream ss;

    if(!nlohmann::json::accept(json_string)){
        ss << "Failed to parse json string: " << json_string << std::endl;
        m_logger.log(Logging::severity_level::warning, ss, "GENTRACE");

        json["error"] = ss.str();
        return;
    }

    json = nlohmann::json::parse(json_string);
    return;
}

std::string HTTPResources::get_bearer_token(const httpserver::http_request &req){
    auto headers = req.get_headers();
    std::string auth(headers["Authorization"]);

    const std::string prefix = "Bearer ";
    if(auth.size() <= prefix.size() || auth.substr(0, prefix.size()) != prefix)
        return "";

    return auth.substr(prefix.size());
}

void HTTPResources::get_req_body_json(const httpserver::http_request &req, nlohmann::json &json){
    std::map<std::string_view, std::string_view, httpserver::http::header_comparator> headers;
    headers = req.get_headers();

    if(headers["Content-Type"] != "application/json"){
        std::string header(headers["Content-Type"]);

        m_logger.log(Logging::severity_level::warning, std::string("Post request Content-Type is not application/json; received header: " + header), "GENTRACE");
        json["error"] = "Post request Content-Type is not application/json; received header: " + header;
        return;
    }

    std::string body_string = std::string(req.get_content());

    parse_json(body_string, json);
}