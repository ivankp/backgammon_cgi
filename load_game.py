#!/usr/bin/env python2

import sys, sqlite3, json

db = sqlite3.connect(sys.argv[1])
cur = db.cursor()

gid = sys.argv[2]
resp = cur.execute('SELECT * FROM games WHERE id='+gid)

game = dict(zip( (x[0] for x in cur.description), resp.fetchone() ))

def lookup(table,field,value):
    return cur.execute(
        'SELECT %s FROM %s WHERE id=%s'%(field,table,value)).fetchone()[0]

def replace(game_field,table,field):
    game[game_field] = lookup(table,field,game[game_field])

replace('player1','users','username')
replace('player2','users','username')

tid = str(game['game_type'])

init = None
for pos in ['pos_init','pos_current']:
    if game[pos] is None:
        if init is None:
            init = lookup('game_types','init',tid)
        game[pos] = init

game['game_type'] = lookup('game_types','name',tid)

json.dump(game,sys.stdout,separators=(',',':'))

db.close()
