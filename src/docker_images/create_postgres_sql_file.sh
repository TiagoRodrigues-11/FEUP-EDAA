#! /bin/sh

# This script is used to create the sql data file for the docker image.

echo "Creating the sql data file for the docker image."
echo "This may take a while..."
echo "Removing old files..."
rm -f spotify-postgres.sql
echo "Creating new sqlite dump..."
sqlite3 ../data/spotify.sqlite .dump > spotify.sql
echo "Converting to utf-8..."
iconv -f utf-16le -t utf-8 spotify.sql > spotify-utf-8.sql
rm -f spotify.sql
echo "Removing pragma..."
sed -i '1d' spotify-utf-8.sql > spotify-utf-8-no-pragma.sql
rm -f spotify-utf-8.sql
echo "Extracting data..."
grep 'INSERT INTO' spotify-utf-8-no-pragma.sql > spotify-data.sql
rm -f spotify-utf-8-no-pragma.sql
echo "Creating new database sql file..."
cat begin-transaction.sql schema.sql spotify-data.sql create-tracks-table-and-end-transaction.sql > spotify-postgres.sql
rm -f spotify-data.sql
echo "Done."
