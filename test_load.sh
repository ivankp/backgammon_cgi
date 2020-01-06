#!/bin/bash

cd cgi

QUERY_STRING="id=$1" \
HTTP_COOKIE="login=$(sqlite3 ../db/database.db 'select cookie from users where id=1')" \
./load_game

