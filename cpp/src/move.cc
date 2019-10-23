#include <cstdlib>
#include <cstring>
#include <cctype>
#include <iostream>
#include <map>

#include "sqlite.hh"
#include "cat.hh"

using namespace std;
using ivanp::cat;

auto get_cookies() {
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

auto sqlmap(ivanp::sqlite& db, const string& sql) {
  map<string,ivanp::sqlite::value> m;
  auto stmt = db.prepare(sql);
  if (stmt.step()) {
    const int n = stmt.column_count();
    for (int i=0; i<n; ++i)
      m[stmt.column_name(i)] = stmt.column_value(i);
  }
  return m;
}

int main() {
  // const char* lenstr = getenv("CONTENT_LENGTH");
  // const size_t len = lenstr==nullptr ? 0 : lexical_cast<size_t>(lenstr);

  cout << "Content-Type: application/json\r\n\r\n";
  // cout << "Content-Type: text/plain\r\n\r\n";

  const string post( istreambuf_iterator<char>(cin), { } );

  const auto cookies = get_cookies();

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
    if (i>=post.size()) return 1;
    const char c = post[i];
    if (isdigit(c)) break;
    moves[i-2] = c - 0x40;
  }

  const char* gid = post.c_str() + i;

  const auto game = sqlmap(db, cat("SELECT * FROM games WHERE id = ", gid));

  cout
    << "{\"user\":" << userid
    << ",\"dice\":" << dice[0] << dice[1]
    << ",\"gid\":" << gid
    << ",\"turn\":" << game.at("turn").as_int()
    << "}" << endl;
}
