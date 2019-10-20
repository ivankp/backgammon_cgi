#include <iostream>
#include <tuple>

#include "sqlite.hh"
#include "cat.hh"

using std::cout;
using std::endl;
using std::get;

int main(int argc, char* argv[]) {
  ivanp::sqlite db(argv[1]);
  const char* username = argv[2];

  auto stmt = db.prepare( ivanp::cat(
    "SELECT g.id, p1.username, p2.username, t.short, g.turn"
    "FROM games AS g"
    "JOIN users AS p1 ON g.player1 = p1.id"
    "JOIN users AS p2 ON g.player2 = p2.id"
    "JOIN game_types AS t ON g.game_type = t.id"
    "WHERE p1.username = \"",username,"\" OR p2.username = \"",username,"\""
  ));
  while (stmt.step()) {
    const int id = stmt.column_int(0);
    const char* u1 = stmt.column_text(1);
    const char* u2 = stmt.column_text(2);
    const char* gt = stmt.column_text(3);
    const char* turn = stmt.column_int(4);

    cout << id <<'|'
         << u1 <<'|'
         << u2 <<'|'
         << gt <<'|'
         << turn << endl;
  }
}
