COMMIT;

BEGIN;

DROP TABLE IF EXISTS tracks_info;

CREATE TABLE tracks_info
AS (
    SELECT tracks.name as name, tracks.id as track_id, artists.name as artist_names, acousticness, danceability, energy, instrumentalness, liveness, loudness, speechiness, tempo, valence, time_signature, mode, tracks.popularity as popularity 
    FROM audio_features
    INNER JOIN tracks ON audio_features.id = tracks.audio_feature_id
    INNER JOIN r_track_artist ON tracks.id = r_track_artist.track_id
    INNER JOIN artists ON r_track_artist.artist_id = artists.id
);

CREATE INDEX track_name_idx ON tracks_info (name);
CREATE INDEX artist_names_idx ON tracks_info (artist_names);
CREATE INDEX popularity_idx ON tracks_info (popularity);

DELETE FROM tracks_info
WHERE name = '';

ALTER TABLE tracks_info ADD COLUMN id SERIAL PRIMARY KEY;

UPDATE tracks_info
SET artist_names = subquery.artist_names
FROM (
  SELECT track_id, string_agg(DISTINCT artist_names, ', ') AS artist_names
  FROM tracks_info
  GROUP BY track_id
) AS subquery
WHERE tracks_info.track_id = subquery.track_id;

DELETE FROM tracks_info
WHERE id NOT IN (
    SELECT MIN(id)
    FROM tracks_info
    GROUP BY track_id
);

ALTER TABLE tracks_info
DROP COLUMN track_id;

COMMIT;
