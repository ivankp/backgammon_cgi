#include <iostream>

#include "cgi.hh"

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << var << std::endl;

using namespace std;
using ivanp::cat;

int main() {
  cout << "Content-Type: application/json\r\n\r\n";

  ivanp::sqlite db("../db/database.db");

  const auto cookies = get_cookies();
  const int userid = cookie_login(db,cookies);

  const auto query = get_query();

  auto game = sqlmap(db, cat(
    "SELECT * FROM games WHERE id = ", query.at("id")));

  const int pl1 = game.at("player1").as_int();
  const int pl2 = game.at("player2").as_int();

  const bool need_flip =
    pl2==userid || (pl1!=userid && game.at("turn").as_int()==2);

  auto lookup = [&](
    const auto& table,
    const auto& field,
    const auto& value
  ){
    auto stmt = db.prepare( cat(
      "SELECT ", field, " FROM ", table, " WHERE id=", value
    ));
    if (!stmt.step()) throw runtime_error(cat("lookup failed for id=",value));
    return stmt.column_value(0);
  };

  auto replace = [&](
    const auto& game_field,
    const auto& table,
    const auto& field
  ) -> decltype(auto) {
    return game.at(game_field)
      = lookup(table,field,game.at(game_field).as_text());
  };

  replace("player1","users","username");
  replace("player2","users","username");

  auto flip = [](auto& pos){ };

  auto& game_type = game.at("game_type");

  ivanp::sqlite::value init;
  for (const string key : {"init","position"}) {
    auto& pos = game.at(key);
    if (!pos) {
      if (!init) {
        init = lookup("game_types","init",game_type.as_text());
        if (need_flip) flip(init);
      }
      pos = init;
    } else if (need_flip) flip(pos);
  }
  game_type = lookup("game_types","name",game_type.as_text());

  cout << "{";
  bool first = true;
  for (const auto& x : game) {
    if (first) first = false;
    else cout << ',';
    cout << '\"' << x.first << "\":";
    switch (x.second.type()) {
      case SQLITE_INTEGER:
      case SQLITE_FLOAT:
        cout << x.second.as_text(); break;
      case SQLITE_NULL:
        cout << "null"; break;
      default:
        cout << '\"' << x.second.as_text() << '\"';
    }
  }
  cout << "}";
}

