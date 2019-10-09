all: database.db

%.db: %.sql
	sqlite3 $@ '.read $<'
	sqlite3 $@ '.read game_types.sql'
	./new_user.sh monads
	./new_user.sh josqvin
	./new_game.sh monads josqvin

clean:
	@rm -fv database.db

