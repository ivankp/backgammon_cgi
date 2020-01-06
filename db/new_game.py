#!/usr/bin/env python

import sys, sqlite3
from random import randint

while True:
    a = randint(1,6)
    b = randint(1,6)
    if a==b: continue
    turn = 0 if (a>b) else 1
    dice = a*10 + b
    break

db = sqlite3.connect(sys.argv[1])
cur = db.cursor()

cur.execute('''INSERT INTO games (
  player1, player2, game_type, turn, dice, cube, white
) SELECT
  (SELECT id FROM users WHERE username = ?),
  (SELECT id FROM users WHERE username = ?),
  (SELECT id FROM game_types WHERE name = "backgammon"),
  ?,
  ?,
  0,
  1
''', (sys.argv[2],sys.argv[3],turn,dice))

db.commit()
db.close()
