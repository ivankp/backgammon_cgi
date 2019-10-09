#!/bin/bash

sqlite3 database.db "DELETE FROM games WHERE id=(SELECT MAX(id) FROM games);"

