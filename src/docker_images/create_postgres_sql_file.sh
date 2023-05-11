#! /bin/sh

# This script is used to create the sql data file for the docker image.

echo "Creating the sql data file for the docker image."
echo "This may take a while..."
echo "Removing old files..."
rm -f spotify-postgres.sql
echo "Creating new sqlite dump..."
sqlite3 ../data/spotify.sqlite .dump > spotify.sql
echo "Converting to utf-8..."
iconv -f latin1 -t utf-8//TRANSLIT//IGNORE spotify.sql > spotify-utf-8.sql
rm -f spotify.sql
echo "Extracting data..."
grep 'INSERT INTO' spotify-utf-8.sql > spotify-data.sql
rm -f spotify-utf-8.sql
echo "Creating new database sql file..."
cat begin-transaction.sql schema.sql spotify-data.sql create-tracks-table-and-end-transaction.sql > spotify-postgres.sql
rm -f spotify-data.sql
echo "Done."
