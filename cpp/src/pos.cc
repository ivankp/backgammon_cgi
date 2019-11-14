#include <iostream>
#include <iomanip>

#include "encode.hh"

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << (var) << std::endl;

using namespace std;

int main(int argc, char** argv) {
  uint8_t board[25][2] = {{0}};
  char pos[10];

  cout << argv[1] << endl;
  /*
  int p = 0;
  for (char c : base64_decode(argv[1])) {
    for (int i=0; i<8; ++i) {
      const bool b = (c >> i) & 1;
      cout << b;
      if (b) ++board[p%25][p/25];
      else ++p;
    }
    cout << ' ';
  }
  */
  { const auto dec = base64_decode(argv[1]);
    for (int i=0; i<10; ++i) {
      for (int j=0; j<8; ++j)
        cout << ((dec[i] >> j) & 1);
      cout << ' ';
    }
    cout << endl;

    position_decode(dec.data(), board);
  }

  cout << '\n' << setfill(' ');
  for (int p=0; p<25; ++p) {
    if (board[p][0] || board[p][1])
      cout << setw(2) << (p+1) << ':'
           << setw(3) << (int)board[p][0]
           << setw(3) << (int)board[p][1] << endl;
  }
  cout << endl;

  position_encode(board,pos);

  for (int j=0; j<10; ++j) {
    for (int i=0; i<8; ++i)
      cout << ( (pos[j] >> i) & 1 );
    cout << ' ';
  }
  cout << endl;

  cout << base64_encode(pos,10) << endl;
}
