#include <iostream>
#include "Song.h"
#include "KdTree.hpp"
#include <random>
#include <chrono>
#include <pqxx/pqxx>

using namespace std;

int main(int argc, char* argv[]){
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <minimum song popularity>" << endl;
        return 1;
    }
    printf("Starting with minimum song popularity %s\n", argv[1]);
    // Make 10 random songs
    std::vector<Song> songs;
    /* std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    Song song1;
    for (int i = 0; i < 1; i++) {
        Song song = Song("Song " + std::to_string(i), "Artist " + std::to_string(i), dis(gen), dis
            (gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen));
        songs.push_back(song);
        song1 = song;
    } */

    //Change hostaddr to your address
    pqxx::connection c("dbname=db user=postgres password=password hostaddr=192.168.208.1 port=5432");

    pqxx::work tx(c);

    char sql[1024] = {0};
    
    // TODO: Change this to a query that only selects songs with a popularity above a certain threshold using new table
    snprintf(sql, 1023, "SELECT tracks.name, artists.name, acousticness, danceability, energy, instrumentalness, liveness, loudness, speechiness, tempo, valence, time_signature, mode " 
                      "FROM audio_features "
                      "INNER JOIN tracks ON audio_features.id = tracks.audio_feature_id "
                      "INNER JOIN r_track_artist ON tracks.id = r_track_artist.track_id "
                      "INNER JOIN artists ON r_track_artist.artist_id = artists.id "
                      "LIMIT %s", argv[1]);

    pqxx::result r = tx.exec(sql);

    tx.commit();

    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        Song song = Song(c[0].as<std::string>(), c[1].as<std::string>(), c[2].as<float>(), c[3].as<float>(), c[4].as<float>(), c[5].as<float>(), c[6].as<float>(), c[7].as<float>(), c[8].as<float>(), c[9].as<float>(), c[10].as<float>(), c[11].as<float>(), c[12].as<float>());
        songs.push_back(song);
        //cout << c[0].as<string>() << endl << c[1].as<string>() << endl << c[2].as<string>() << endl;
    }

    cout << "Songs: " << songs.size() << endl;

    songs.shrink_to_fit();
    /* // Print the songs
    for (size_t i = 0; i < songs.size(); i++) {
        std::cout << songs[i].getName() << " " << songs[i].getArtist() << " " << songs[i][0] << " " << songs[i][1] << " " << songs[i][2] << " " << songs[i][3] << " " << songs[i][4] << " " << songs[i][5] << " " << songs[i][6] << " " << songs[i][7] << " " << songs[i][8] << " " << songs[i][9] << " " << songs[i][10] << " " << songs[i][11] << std::endl;
    } */

    cout << "Building KdTree with " << songs.size() << " nodes..." << endl;

    auto start = std::chrono::system_clock::now();

    // Make a KdTree
    KdTree<Song> tree = KdTree<Song>(songs);

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    
    cout << "Finished in: " << elapsed_seconds.count() << "s" << endl;

   /*  Song neighbour = tree.nearestNeighborSearch(*(tree.getRoot()), song1, 0, INFINITY);
    std::cout << "Nearest neighbour: " << neighbour.getName() << " " << neighbour.getArtist() << " " << neighbour[0] << " " << neighbour[1] << " " << neighbour[2] << " " << neighbour[3] << " " << neighbour[4] << " " << neighbour[5] << " " << neighbour[6] << " " << neighbour[7] << " " << neighbour[8] << " " << neighbour[9] << " " << neighbour[10] << " " << neighbour[11] << std::endl; */
}