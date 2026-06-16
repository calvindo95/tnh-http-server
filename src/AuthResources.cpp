#include <AuthResources.h>

#include <sstream>
#include <iomanip>

std::string AuthResources::bytes_to_hex(const unsigned char* bytes, size_t len){
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for(size_t i = 0; i < len; i++)
        ss << std::setw(2) << static_cast<int>(bytes[i]);
    return ss.str();
}

bool AuthResources::hex_to_bytes(const std::string& hex, std::vector<unsigned char>& out){
    if(hex.size() % 2 != 0) return false;
    out.resize(hex.size() / 2);
    for(size_t i = 0; i < out.size(); i++){
        unsigned int byte;
        if(sscanf(hex.c_str() + 2 * i, "%02x", &byte) != 1) return false;
        out[i] = static_cast<unsigned char>(byte);
    }
    return true;
}
