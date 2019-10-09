#!/bin/bash

sqlite3 database.db "INSERT INTO users (username) VALUES ('$1');"

