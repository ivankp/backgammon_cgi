#include <cstdlib>
#include <cstring>
#include <cctype>
#include <iostream>
#include <map>

#include "sqlite.hh"
#include "cat.hh"

using namespace std;
using ivanp::cat;

map<string,string> get_cookies() {
  map<string,string> cookies_map;
  const char* str = getenv("HTTP_COOKIE");
  const char* end = str + strlen(str);
  string key, val;
  for (const char* s=str; s<end; ) {
    switch (*s) {
      case '=':
        key.assign(str,s);
        str = ++s;
        continue;
      case ';':
        val.assign(str,s);
        while (*++s == ' ') { }
        str = s;
        break;
      default:
        if (++s!=end) continue;
        else val.assign(str,s);
    }
    cookies_map[std::move(key)] = std::move(val);
  }
  return cookies_map;
}

int main() {
  // const char* lenstr = getenv("CONTENT_LENGTH");
  // const size_t len = lenstr==nullptr ? 0 : lexical_cast<size_t>(lenstr);

  istreambuf_iterator<char> begin(cin), end;
  const string post(begin, end);

  const auto cookies = get_cookies();

  // cout << "Content-Type: application/json\r\n\r\n";
  // cout << "[\"" << post <<"\",\"" << cookies.at("login") << "\"]";

  ivanp::sqlite db("../db/database.db");

  int userid;
  { auto stmt = db.prepare( cat(
      "SELECT id"
      " FROM users"
      " WHERE cookie = \"",cookies.at("login"),"\""
    ));
    if (!stmt.step()) return 1;
    userid = stmt.column_int(0);
  }

  const int dice[2] = { post[0]-0x30, post[1]-0x30 };
  int moves[4] = { 0 };

  size_t i = 2;
  for (; ; ++i) {
    const char c = post[i];
    if (isdigit(c)) break;
    else if (i>=post.size()) return 1;
    moves[i-2] = c - 0x40;
  }

  const char* gid = post.c_str() + i;

  cout << "Content-Type: application/json\r\n\r\n";
  cout << "{"
    "\"user\":" << userid << ","
    "\"dice\":" << dice[0] << dice[1] << ","
    "\"gid\":" << gid << "}" << endl;
}
