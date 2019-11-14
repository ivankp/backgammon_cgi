#ifndef BG_ENCODE_HH
#define BG_ENCODE_HH

#include <cstring>
#include <cstdint>
#include <vector>
#include <string>

const char* base64_encode_table =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const uint8_t base64_decode_table[128] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
  0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
  0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
  0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
  0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
  0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
  0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
  0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
  0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

std::string base64_encode(const char* ptr, size_t len) {
  const char* end = ptr + len;
  len *= 8;
  std::string out(len/6 + !!(len%6), '\0');
  char* o = &out[0];
  uint8_t d=0, step=0, mask=0;
  uint8_t byte;
  for (; ptr!=end; ++ptr) {
    byte = *ptr;
    (mask >>= 2) |= (3u<<4);
    step += 2;
    d |= (byte >> step);
    *(o++) = base64_encode_table[d];
    d = (byte << (6-step)) & mask;
    if (step==6) {
      *(o++) = base64_encode_table[d];
      step = 0;
      mask = 0;
      d = 0;
    }
  }
  if (step) *(o++) = base64_encode_table[d];
  return out;
}

std::vector<char> base64_decode(const char* ptr, size_t len) {
  const char* end = ptr + len;
  len *= 6;
  std::vector<char> out(len/8 + !!(len%8));
  char* o = out.data();
  uint8_t c, d, step = 0;
  char byte;
  for (; ptr!=end; ++ptr) {
    if ((c=*ptr)>>7 || (d=base64_decode_table[c])==0xFF) break;
    step += 2;
    if (step>2) {
      byte |= (d >> (8-step));
      *(o++) = byte;
    }
    if (step<8) byte = (d << step);
    else step = 0;
  }
  if (step) *(o++) = byte;
  return out;
}
std::vector<char> base64_decode(const char* str) {
  return base64_decode(str,strlen(str));
}
std::vector<char> base64_decode(const std::string& str) {
  return base64_decode(str.c_str(),str.size());
}

void position_encode(const uint8_t (*board)[2], char* b) {
  unsigned i = 0; // index of current bit
  unsigned c = 0;
  for (int pl=0; ; pl=!pl) {
    for (auto *pt=board, *end=board+25; pt!=end; ++pt) {
      auto n = (*pt)[pl];
      do {
        c >>= 1;
        if (n) c |= 1u<<7;
        // else c &= ~(1<<7);
        if (++i == 8) { *b=c; ++b; i=0; c=0; }
      } while (n--);
    }
    if (pl) break;
  }
}

void position_decode(const char* b, uint8_t (*board)[2]) {
  int p = 0;
  for (const char* end=b+10; b!=end; ++b) {
    for (int i=0; i<8; ++i) {
      if ((*b >> i) & 1) ++board[p%25][p/25];
      else ++p;
    }
  }
}

#endif
