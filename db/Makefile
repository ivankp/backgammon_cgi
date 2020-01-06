DB = database.db

all: $(DB)

%.db: %.sql
	sqlite3 $@ '.read $<'
	sqlite3 $@ '.read game_types.sql'
	./new_user.py $@ monads
	./new_user.py $@ josqvin
	sqlite3 $@ \
	  'INSERT INTO games VALUES(1,1,2,1,NULL,1,NULL,NULL,NULL,54,0,NULL,NULL,1)'
	sqlite3 $@ \
	  'INSERT INTO games VALUES(2,1,2,1,NULL,0,"YA4+AAZ8Dj4ABg",NULL,NULL,13,1,NULL,NULL,1)'
	sqlite3 $@ \
	  'INSERT INTO games VALUES(3,1,2,1,NULL,0,NULL,NULL,NULL,21,0,NULL,NULL,1)'
	sqlite3 $@ \
	  'INSERT INTO games VALUES(4,2,1,1,NULL,0,"YA4+AAZ8Dj4ABg",NULL,NULL,13,0,NULL,NULL,1)'
	# ./new_game.py $@ monads josqvin
	# ./new_game.py $@ monads josqvin
	# ./new_game.py $@ monads josqvin

clean:
	@rm -fv $(DB)

dump:
	@sqlite3 $(DB) .dump

