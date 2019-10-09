#!/bin/bash

sqlite3 database.db "INSERT INTO games (
  player1, player2, game_type, cube
) SELECT
  (SELECT id FROM users WHERE username = '$1'),
  (SELECT id FROM users WHERE username = '$2'),
  (SELECT id FROM game_types WHERE name = 'backgammon'),
  0
;"

