DB = database.db

all: $(DB)

%.db: %.sql
	sqlite3 $@ '.read $<'
	sqlite3 $@ '.read game_types.sql'
	./new_user.py $@ monads
	./new_user.py $@ josqvin
	./new_game.py $@ monads josqvin
	./new_game.py $@ monads josqvin
	./new_game.py $@ monads josqvin

clean:
	@rm -fv $(DB)

dump:
	@sqlite3 $(DB) .dump

