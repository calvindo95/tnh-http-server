#include <AuthSession.h>

std::shared_ptr<httpserver::http_response> auth_session::render(const httpserver::http_request& req){
    std::string session_id = HTTPResources::get_bearer_token(req);

    if(session_id.empty()){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Missing or invalid Authorization header\"}", 401, "application/json"));
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
