#include <RegisterUser.h>

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <vector>

// Returns "sha256:<hex_salt>:<hex_hash>" or "" on failure
std::string register_user::hash_password(const std::string& password){
    unsigned char salt[16];
    if(RAND_bytes(salt, sizeof(salt)) != 1) return "";

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if(!ctx) return "";

    unsigned char hash[32];
    unsigned int hash_len = 0;

    bool ok = EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr)     &&
              EVP_DigestUpdate(ctx, salt, sizeof(salt))          &&
              EVP_DigestUpdate(ctx, password.c_str(), password.size()) &&
              EVP_DigestFinal_ex(ctx, hash, &hash_len);

    EVP_MD_CTX_free(ctx);

    if(!ok || hash_len != 32) return "";

    return "sha256:" + bytes_to_hex(salt, sizeof(salt)) + ":" + bytes_to_hex(hash, hash_len);
}

std::shared_ptr<httpserver::http_response> register_user::render(const httpserver::http_request& req){
    nlohmann::json req_json;
    HTTPResources::get_req_body_json(req, req_json);

    if(req_json.is_null() || req_json.contains("error")){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Error parsing request json\"}", 400, "application/json"));
    }

    if(!req_json.contains("Username") || !req_json.contains("Password") ||
       !req_json["Username"].is_string() || !req_json["Password"].is_string()){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Missing Username or Password\"}", 400, "application/json"));
    }

    std::string username = req_json["Username"].get<std::string>();
    std::string password = req_json["Password"].get<std::string>();

    if(username.empty() || username.length() > 255){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Username must be between 1 and 255 characters\"}", 400, "application/json"));
    }

    if(password.empty()){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Password must not be empty\"}", 400, "application/json"));
    }

    std::string password_hash = hash_password(password);
    if(password_hash.empty()){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Internal server error\"}", 500, "application/json"));
    }

    int user_id = m_dbq.create_user(username, password_hash);

    if(user_id == 0){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Username already exists\"}", 409, "application/json"));
    }
    if(user_id < 0){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Internal server error\"}", 500, "application/json"));
    }

    m_logger.log(Logging::severity_level::info,
        "Registered new user: " + username, "AUTH");

    nlohmann::json resp;
    resp["user_id"]  = user_id;
    resp["Username"] = username;
    return std::shared_ptr<httpserver::http_response>(
        new httpserver::string_response(resp.dump(), 201, "application/json"));
}
