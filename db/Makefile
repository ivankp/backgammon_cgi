DB = database.db

all: $(DB)

%.db: %.sql
	sqlite3 $@ '.read $<'
	sqlite3 $@ '.read game_types.sql'
	./new_user.sh monads
	./new_user.sh josqvin
	./new_game.py $@ monads josqvin
	./new_game.py $@ monads josqvin
	./new_game.py $@ monads josqvin

clean:
	@rm -fv $(DB)

dump:
	@sqlite3 $(DB) .dump

