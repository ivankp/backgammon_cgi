#include "cgi.hh"

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <iostream>
#include <stdexcept>
// #include <boost/lexical_cast.hpp>

using namespace std;
using ivanp::cat;

string get_post_str() {
  // const char* lenstr = getenv("CONTENT_LENGTH");
  // const size_t len = lenstr==nullptr ? 0 : boost::lexical_cast<size_t>(lenstr);
  return { istreambuf_iterator<char>(cin), { } };
}

str_map get_query() {
  str_map m;
  const char* str = getenv("QUERY_STRING");
  const char* end = str + strlen(str);
  string key, val;
  for (const char* s=str; s<end; ) {
    switch (*s) {
      case '=':
        key.assign(str,s);
        str = ++s;
        continue;
      case '&':
        val.assign(str,s);
        while (*++s == ' ') { }
        str = s;
        break;
      default:
        if (++s!=end) continue;
        else val.assign(str,s);
    }
    m[std::move(key)] = std::move(val);
  }
  return m;
}

str_map get_cookies() {
  str_map m;
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
    m[std::move(key)] = std::move(val);
  }
  return m;
}

sql_map sqlmap(ivanp::sqlite& db, const string& sql) {
  sql_map m;
  auto stmt = db.prepare(sql);
  if (stmt.step()) {
    const int n = stmt.column_count();
    for (int i=0; i<n; ++i)
      m[stmt.column_name(i)] = stmt.column_value(i);
  }
  return m;
}

int cookie_login(ivanp::sqlite& db, const str_map& cookies) {
  auto stmt = db.prepare( cat(
    "SELECT id"
    " FROM users"
    " WHERE cookie = \"",cookies.at("login"),"\""
  ));
  if (!stmt.step()) throw runtime_error("cookie login failed");
  return stmt.column_int(0);
}
