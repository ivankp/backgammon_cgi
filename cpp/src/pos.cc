#include <cstring>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <vector>
// #include <bitset>
#include <string>

// const pos_bin = atob(game.position);
/*
function set_board() {
  g_checkers.empty();
  g_checkers.svg('g');
  borneoff = [15,15];
  let n = 0, p = 0;
  loop_i: for (let i=0; i<pos_bin.length; ++i) {
    const c = pos_bin.charCodeAt(i);
    let mask = 1;
    for (let j=0; j<8; ++j) {
      if (c & mask) {
        const checker = draw_checker(p,n);
        --borneoff[p];
      } else if (n==24) {
        if (p) break loop_i;
        else { p = 1-p; n = 47-n; }
      } else {
        if (!p) {
          g_checkers.svg('g');
          ++n;
        } else {
          if (!n) n = 24;
          else --n;
        }
      }
      mask = mask << 1;
    }
  }
*/

const char* encode_table =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const uint8_t decode_table[128] = {
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

std::vector<char> decode(const char* ptr, size_t len) {
  const void* end = ptr + len;
  len *= 6;
  std::vector<char> out(len/8 + !!(len%8));
  char* o = out.data();
  uint8_t step = 0;
  char byte;
  for (uint8_t c, d; ptr!=end; ++ptr) {
    if ((c=*ptr)>>7 || (d=decode_table[c])==0xFF) break;
    step += 2;
    if (step>2) {
      byte += (d >> (8-step));
      *(o++) = byte;
    }
    if (step<8) byte = (d << step);
    else step = 0;
  }
  if (step) *(o++) = byte;
  return out;
}
std::vector<char> decode(const char* str) {
  return decode(str,strlen(str));
}
std::vector<char> decode(const std::string& str) {
  return decode(str.c_str(),str.size());
}

using namespace std;

int main(int argc, char** argv) {
  int board[25][2] = {0};

  cout << argv[1] << endl;
  int p = 0;
  for (char c : decode(argv[1])) {
    for (int i=0; i<8; ++i) {
      const bool b = (c >> i) & 1;
      cout << b;
      if (b) ++board[p%25][p/25];
      else ++p;
    }
  }
  cout << endl;

  cout << '\n' << setfill(' ');
  for (p=0; p<25; ++p) {
    if (board[p][0] || board[p][1])
      cout << setw(2) << (p+1) << ':'
           << setw(3) << board[p][0]
           << setw(3) << board[p][1] << endl;
  }

}
