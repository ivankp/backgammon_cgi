#ifndef BG_ENCODE_HH
#define BG_ENCODE_HH

#include <cstdint>
#include <vector>
#include <string>

std::string base64_encode(const char* ptr, size_t len);
std::vector<char> base64_decode(const char* ptr, size_t len);
std::vector<char> base64_decode(const char* str);
std::vector<char> base64_decode(const std::string& str);

void position_encode(const uint8_t (*board)[2], char* b);
void position_decode(const char* b, uint8_t (*board)[2]);

#endif
