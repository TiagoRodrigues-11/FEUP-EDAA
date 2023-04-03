import sqlite3

con = sqlite3.connect('data/spotify.sqlite')
cur = con.cursor()

# Create a table with all the tracks and their artists - needed because inner join takes too long
#cur.execute('''
#            CREATE TABLE IF NOT EXISTS track_artist AS
#            SELECT tracks.*, artists.name AS artist_name
#            FROM tracks
#            INNER JOIN r_track_artist ON tracks.id = r_track_artist.track_id
#            INNER JOIN artists ON r_track_artist.artist_id = artists.id
#            ''')

for row in cur.execute('SELECT name FROM track_artist WHERE artist_name = "Taylor Swift"'):
    print(row[0])
    
"""id = cur.execute('SELECT id FROM artists WHERE name = "Taylor Swift"').fetchone()[0]
 
print(id)
    
tracks_ids = [x[0] for x in cur.execute('SELECT track_id FROM r_track_artist WHERE artist_id = ?', (id,)).fetchall()]

print(tracks_ids)

for track_id in tracks_ids:
    print(cur.execute('SELECT name FROM tracks WHERE id = ?', (track_id,)).fetchone()[0]) """

con.close()