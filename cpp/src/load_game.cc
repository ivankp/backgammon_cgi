#include <iostream>
#include <type_traits>
#include <iterator>

#include "cgi.hh"
#include "encode.hh"
// #include "streamer.hh"

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << var << std::endl;

using namespace std;
using ivanp::cat;

uint8_t board[25][2] = {{0}};

void flip_pos(ivanp::sqlite::variant& v) {
  auto dec = base64_decode(get<string>(v));
  auto* pos = &dec[0];
  position_decode(pos,board);

  for (unsigned i=0; i<25; ++i) // swap black and white
    swap(board[i][0],board[i][1]);

  position_encode(board,pos);
  v = base64_encode(pos,10);
}

int main() {
  cout << "Content-Type: application/json\r\n\r\n";

  ivanp::sqlite db("../db/database.db");

  const int userid = cookie_login(db,get_cookies());

  const auto query = get_query();

  auto game = db.query(cat(
    "SELECT * FROM games WHERE id=", query.at("id")));

  int& turn = get<int>(game.at("turn"));
  int pl_id[2] = {
    get<int>(game.at("player1")),
    get<int>(game.at("player2"))
  };

  const bool flip = pl_id[1]==userid || (pl_id[0]!=userid && turn==1);

  if (flip) {
    swap(pl_id[0],pl_id[1]);
    auto& white = get<int>(game.at("white"));
    white = !white;
    turn = !turn;
  }

  auto lookup = [&](
    const auto& table,
    const auto& field,
    const auto& value
  ){
    auto stmt = db.prepare( cat(
      "SELECT ", field, " FROM ", table, " WHERE id=", value
    ));
    if (!stmt.step()) throw runtime_error(cat(
      "lookup failed in table ",table," for id ",value ));
    return stmt.column_value(0).as_variant();
  };

  string pl_name[2] = {
    get<string>(lookup("users","username",pl_id[0])),
    get<string>(lookup("users","username",pl_id[1]))
  };

  auto& game_type = game.at("game_type");

  auto& init = game.at("init");
  if (!init.index())
    init = lookup("game_types","init",get<int>(game_type));
  if (flip) flip_pos(init);
  auto& pos = game.at("position");
  if (!pos.index()) pos = init;
  else if (flip) flip_pos(pos);

  game_type = lookup("game_types","name",get<int>(game_type));

  cout << "{\"turn\":"
    << ( pl_id[turn]==userid ? "true" : "false" )
    << ",\"players\":["
       "["<< pl_id[0] <<",\""<< pl_name[0] <<"\"],"
       "["<< pl_id[1] <<",\""<< pl_name[1] <<"\"]]";
  for (const auto& x : game) {
    for (const char* name : {"turn","init","player1","player2"})
      if (x.first == name) goto next;
    cout << ",\"" << x.first << "\":";
    visit([](auto&& x){
      using type = decay_t<decltype(x)>;
      if constexpr (is_null_pointer_v<type>) {
        cout << "null";
      } else if constexpr (is_arithmetic_v<type>) {
        cout << x;
      } else {
        cout << '\"';
        copy(x.begin(), x.end(), ostream_iterator<char>(cout));
        cout << '\"';
      }
    }, x.second);
    next: ;
  }
  cout << "}\n";
}

