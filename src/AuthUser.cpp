#include <AuthUser.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

#include <vector>

// Stored password format: sha256:<hex_salt>:<hex_hash>
// where hex_hash = SHA-256(salt_bytes || password)

bool auth_user::verify_password(const std::string& password, const std::string& stored_hash){
    // Parse: sha256:<hex_salt>:<hex_hash>

    // Checks if first 7 chars are sha256:
    if(stored_hash.substr(0, 7) != "sha256:") return false;

    // Checks if second colon exists
    size_t colon = stored_hash.find(':', 7);
    if(colon == std::string::npos) return false;

    // Extract salt between pos 7 and second colon
    // Extract hash after second colon
    std::string hex_salt = stored_hash.substr(7, colon - 7);
    std::string hex_hash = stored_hash.substr(colon + 1);

    // Check if hash is well formed (salt string can be decoded to bytes, hash can be decoded to bytes, and hash is 32 bytes)
    std::vector<unsigned char> salt;
    std::vector<unsigned char> expected;
    if(!hex_to_bytes(hex_salt, salt) || !hex_to_bytes(hex_hash, expected) || expected.size() != 32)
        return false;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if(!ctx) return false;

    unsigned char computed[32];
    unsigned int computed_len = 0;

    // Compute hash of salt+pw
    bool ok = EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr)  &&
              EVP_DigestUpdate(ctx, salt.data(), salt.size()) &&
              EVP_DigestUpdate(ctx, password.c_str(), password.size()) &&
              EVP_DigestFinal_ex(ctx, computed, &computed_len);

    EVP_MD_CTX_free(ctx);

    if(!ok || computed_len != 32) return false;

    // returns bool if computed salt+hash matches DB hash
    return CRYPTO_memcmp(computed, expected.data(), 32) == 0;
}

std::string auth_user::generate_session_id(){
    unsigned char bytes[32];
    if(RAND_bytes(bytes, sizeof(bytes)) != 1) return "";
    return bytes_to_hex(bytes, sizeof(bytes));
}

std::shared_ptr<httpserver::http_response> auth_user::render(const httpserver::http_request& req){
    nlohmann::json req_json;
    HTTPResources::get_req_body_json(req, req_json);

    if(req_json.is_null() || req_json.contains("error")){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Error parsing request json\"}", 400, "application/json"));
    }

    if(!req_json.contains("Username") || !req_json.contains("Password") ||
       !req_json["Username"].is_string() || !req_json["Password"].is_string()){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Missing username or password\"}", 400, "application/json"));
    }

    std::string username = req_json["Username"].get<std::string>();
    std::string password = req_json["Password"].get<std::string>();

    if(username.empty() || username.length() > 255 || password.empty()){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Invalid credentials\"}", 401, "application/json"));
    }

    std::string stored_hash;
    int user_id = m_dbq.get_user(username, stored_hash);

    // Use the same error message whether user doesn't exist or password is wrong
    // to prevent user enumeration
    if(user_id <= 0 || !verify_password(password, stored_hash)){
        m_logger.log(Logging::severity_level::warning,
            "Failed auth attempt for user: " + username, "AUTH");
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Invalid credentials\"}", 401, "application/json"));
    }

    std::string session_id = generate_session_id();
    if(session_id.empty() || !m_dbq.create_session(user_id, session_id)){
        return std::shared_ptr<httpserver::http_response>(
            new httpserver::string_response("{\"error\":\"Internal server error\"}", 500, "application/json"));
    }

    m_logger.log(Logging::severity_level::info,
        "Successful authentication for user: " + username, "AUTH");

    nlohmann::json resp;
    resp["session_id"] = session_id;
    return std::shared_ptr<httpserver::http_response>(
        new httpserver::string_response(resp.dump(), 200, "application/json"));
}
