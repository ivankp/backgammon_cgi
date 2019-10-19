#!/usr/bin/env python

import sys, sqlite3, random, string

db = sqlite3.connect(sys.argv[1])
cur = db.cursor()

chars = string.ascii_uppercase + string.ascii_lowercase + string.digits
sql = 'INSERT INTO users (username, cookie) VALUES ("%s",?)' % sys.argv[2]

while True:
    try:
        cur.execute(sql,(''.join(random.choice(chars) for _ in range(16)),))
    except sqlite3.Error as e:
        what = e.args[0]
        if what=='column cookie is not unique': continue
        else:
            raise
            break
    break

db.commit()
db.close()
