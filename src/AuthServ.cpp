#include <AuthServ.h>

#include <chrono>
#include <thread>

auth_serv::auth_serv(){
    std::thread t(&auth_serv::cleanup_thread, this);
    t.detach();
}

void auth_serv::cleanup_thread() noexcept{
    DBQ dbq;
    while(true){
        std::this_thread::sleep_for(std::chrono::minutes(1));
        dbq.delete_expired_sessions();
        m_logger.log(Logging::severity_level::trace, "Session cleanup complete", "AUTH");
    }
}

std::shared_ptr<httpserver::http_response> auth_serv::render(const httpserver::http_request& req){
    std::string session_id = HTTPResources::get_bearer_token(req);

    if(session_id.empty()){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Missing or invalid Authorization header\"}", 401, "application/json"));
    }

    int result = m_dbq.delete_session(session_id);

    if(result == 0){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Session not found\"}", 404, "application/json"));
    }
    if(result < 0){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Internal server error\"}", 500, "application/json"));
    }

    m_logger.log(Logging::severity_level::info, "Session signed out", "AUTH");

    return std::shared_ptr<httpserver::http_response>(
        new httpserver::string_response("{\"message\":\"Signed out\"}", 200, "application/json"));
}
