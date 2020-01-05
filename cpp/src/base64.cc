#include <iostream>
#include <iomanip>
using namespace std;

const char encode_table[65] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int decode_table[128];

int main(int argv, char** argc) {
  cout << "const char* encode_table =\n"
    "  \"" << encode_table << "\";\n";
  cout << "const char decode_table[128] = {";

  for (int i=0; i<128; ++i)
    decode_table[i] = 0xFF;

  for (int i=0; i<64; ++i)
    decode_table[encode_table[i]] = i;

  for (int i=0; i<128; ++i) {
    cout << (i%8 ? ", " : i ? ",\n  " : "\n  ");
    cout << "0x" << hex << uppercase << setw(2) << setfill('0') << decode_table[i];
  }
  cout << "\n};\n";
}
