PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS users (
  id INTEGER PRIMARY KEY,
  username TEXT UNIQUE NOT NULL,
  cookie TEXT UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS game_types (
  id INTEGER PRIMARY KEY,
  name TEXT UNIQUE NOT NULL,
  short TEXT UNIQUE NOT NULL,
  init TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS games (
  id INTEGER PRIMARY KEY,
  player1 INTEGER REFERENCES users,
  player2 INTEGER REFERENCES users,
  game_type INTEGER REFERENCES game_types,
  match INTEGER REFERENCES matches,
  white INTEGER CHECK (white in (0,1)),
  init TEXT, -- if null then same as type
  position TEXT,
  moves TEXT,
  dice INTEGER,
  turn INTEGER CHECK (turn in (0,1)),
  winner INTEGER CHECK (winner in (NULL,0,1)),
  cube INTEGER,
  gammon INTEGER CHECK (gammon in (1,2,3))
);

CREATE TABLE IF NOT EXISTS matches (
  id INTEGER PRIMARY KEY,
  game INTEGER REFERENCES games,
  npoints INTEGER,
  points1 INTEGER DEFAULT 0,
  points2 INTEGER DEFAULT 0
);
