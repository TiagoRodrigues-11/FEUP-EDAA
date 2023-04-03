CREATE TABLE albums (
    id TEXT PRIMARY KEY,
    name TEXT,
    album_group TEXT,
    album_type TEXT,
    release_date NUMERIC,
    popularity NUMERIC
    );

CREATE TABLE artists (
    name TEXT,
    id TEXT PRIMARY KEY,
    popularity NUMERIC,
    followers NUMERIC
    );

CREATE TABLE audio_features (
    id TEXT PRIMARY KEY,
    acousticness NUMERIC,
    analysis_url TEXT,
    danceability NUMERIC,
    duration NUMERIC,
    energy NUMERIC,
    instrumentalness NUMERIC,
    key NUMERIC,
    liveness NUMERIC,
    loudness NUMERIC,
    mode NUMERIC,
    speechiness NUMERIC,
    tempo NUMERIC,
    time_signature NUMERIC,
    valence NUMERIC
    );

CREATE TABLE genres (
    id TEXT PRIMARY KEY
    );

CREATE TABLE tracks(
    id TEXT PRIMARY KEY,
    disc_number NUMERIC,
    duration NUMERIC,
    explicit NUMERIC,
    audio_feature_id TEXT,
    name TEXT,
    preview_url TEXT,
    track_number NUMERIC,
    popularity NUMERIC,
    is_playable NUMERIC,
    FOREIGN KEY (audio_feature_id) REFERENCES audio_features(id) DEFERRABLE INITIALLY IMMEDIATE
    );

CREATE TABLE r_albums_artists (
    album_id TEXT,
    artist_id TEXT,
    FOREIGN KEY (album_id) REFERENCES albums(id) DEFERRABLE INITIALLY IMMEDIATE,
    FOREIGN KEY (artist_id) REFERENCES artists(id) DEFERRABLE INITIALLY IMMEDIATE,
    PRIMARY KEY (album_id, artist_id)
    );

CREATE TABLE r_albums_tracks (
    album_id TEXT,
    track_id TEXT,
    FOREIGN KEY (album_id) REFERENCES albums(id) DEFERRABLE INITIALLY IMMEDIATE,
    FOREIGN KEY (track_id) REFERENCES tracks(id) DEFERRABLE INITIALLY IMMEDIATE,
    PRIMARY KEY (album_id, track_id)
    );

CREATE TABLE r_artist_genre (
    genre_id TEXT,
    artist_id TEXT,
    FOREIGN KEY (genre_id) REFERENCES genres(id) DEFERRABLE INITIALLY IMMEDIATE,
    FOREIGN KEY (artist_id) REFERENCES artists(id) DEFERRABLE INITIALLY IMMEDIATE,
    PRIMARY KEY (genre_id, artist_id)
    );

CREATE TABLE r_track_artist (
    track_id TEXT,
    artist_id TEXT,
    FOREIGN KEY (track_id) REFERENCES tracks(id) DEFERRABLE INITIALLY IMMEDIATE,
    FOREIGN KEY (artist_id) REFERENCES artists(id) DEFERRABLE INITIALLY IMMEDIATE,
    PRIMARY KEY (track_id, artist_id)
    );

