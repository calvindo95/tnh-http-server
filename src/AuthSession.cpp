#include <AuthSession.h>

std::shared_ptr<httpserver::http_response> auth_session::render(const httpserver::http_request& req){
    nlohmann::json req_json;
    HTTPResources::get_req_body_json(req, req_json);

    if(req_json.is_null() || req_json.contains("error")){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Error parsing request json\"}", 400, "application/json"));
    }

    if(!req_json.contains("session_id") || !req_json["session_id"].is_string()){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Missing session_id\"}", 400, "application/json"));
    }

    std::string session_id = req_json["session_id"].get<std::string>();

    if(session_id.empty()){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Missing session_id\"}", 400, "application/json"));
    }

    int user_id = m_dbq.validate_session(session_id);

    if(user_id < 0){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Internal server error\"}", 500, "application/json"));
    }

    if(user_id == 0){
        // Session not found or expired — tell the frontend to invalidate
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"valid\":false}", 401, "application/json"));
    }

    return std::shared_ptr<httpserver::http_response>(
        new httpserver::string_response("{\"valid\":true}", 200, "application/json"));
}
