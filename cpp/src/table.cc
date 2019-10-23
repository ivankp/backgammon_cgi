#include <iostream>
#include <cstring>

#include "sqlite.hh"
#include "cat.hh"

using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
  ivanp::sqlite db(argv[1]);
  const char* username = argv[2];

  auto stmt = db.prepare( ivanp::cat(
    "SELECT g.id, p1.username, p2.username, t.short, g.turn"
    " FROM games AS g"
    " JOIN users AS p1 ON g.player1 = p1.id"
    " JOIN users AS p2 ON g.player2 = p2.id"
    " JOIN game_types AS t ON g.game_type = t.id"
    " WHERE g.winner IS NULL AND"
    " (p1.username = \"",username,"\" OR p2.username = \"",username,"\")"
  ));
  bool first = true;
  cout << '[';
  while (stmt.step()) {
    const int id = stmt.column_int(0);
    const char* p[] = { stmt.column_text(1), stmt.column_text(2) };
    const char* gt = stmt.column_text(3);
    const int turn = stmt.column_int(4)-1;

    const bool myturn = !strcmp(username,p[turn]);
    const char* opp = p[myturn ^ turn];

    if (first) first = false;
    else cout << ',';
    cout<<'['<<(!myturn)<<",\""<<opp<<"\",\""<<gt<<"\","<<id<<']';
  }
  cout << ']';
}
