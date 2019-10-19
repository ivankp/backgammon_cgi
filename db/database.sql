PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS users (
  id INTEGER PRIMARY KEY,
  username TEXT UNIQUE NOT NULL,
  cookie TEXT UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS game_types (
  id INTEGER PRIMARY KEY,
  name TEXT UNIQUE NOT NULL,
  short TEXT NOT NULL,
  init TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS games (
  id INTEGER PRIMARY KEY,
  player1 INTEGER REFERENCES users,
  player2 INTEGER REFERENCES users,
  game_type INTEGER REFERENCES game_types,
  match INTEGER REFERENCES matches,
  pos_init TEXT, -- if null then same as type
  pos_current TEXT,
  moves TEXT,
  dice INTEGER,
  turn INTEGER, -- 1 = player1, 2 = player2
  finished INTEGER DEFAULT 0,
  cube INTEGER DEFAULT 0, -- 0 = not in play
  mult INTEGER DEFAULT 1 -- gammon=2, backgammon=3
);

CREATE TABLE IF NOT EXISTS matches (
  id INTEGER PRIMARY KEY,
  game INTEGER REFERENCES games,
  npoints INTEGER,
  points1 INTEGER DEFAULT 0,
  points2 INTEGER DEFAULT 0
);
