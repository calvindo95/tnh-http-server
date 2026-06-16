#pragma once

#include <string>
#include <vector>

class AuthResources {
protected:
    static std::string bytes_to_hex(const unsigned char* bytes, size_t len);
    static bool hex_to_bytes(const std::string& hex, std::vector<unsigned char>& out);
};
