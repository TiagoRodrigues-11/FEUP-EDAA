#include <iostream>
#include <fstream>
#include "Song.h"
#include "KdTree.hpp"
#include <random>
#include <chrono>
#include <queue>
#include <pqxx/pqxx>

using namespace std;

void printSong(Song *song) {
    // Name - Author
    std::cout << song->getName() << " - " << song->getArtist() << std::endl;
    // Other attributes
    std::cout << "Acousticness: " << (*song)[0] << std::endl;
    std::cout << "Danceability: " << (*song)[1] << std::endl;
    std::cout << "Energy: " <<  (*song)[2] << std::endl;
    std::cout << "Instrumentalness: " <<  (*song)[3] << std::endl;
    std::cout << "Liveness: " <<  (*song)[4] << std::endl;
    std::cout << "Loudness: " <<  (*song)[5] << std::endl;
    std::cout << "Speechiness: " <<  (*song)[6] << std::endl;
    std::cout << "Tempo: " <<  (*song)[7] << std::endl;
    std::cout << "Valence: " <<  (*song)[8] << std::endl;
    std::cout << "Time Signature: " <<  (*song)[9] << std::endl;
    std::cout << "Mode: " << (*song)[10] << std::endl;
    std::cout << std::endl;
}


double distance(Song point1, Song point2)
{
    double sum = 0;
    for (int i = 0; i < point1.dimensions(); i++)
    {
        sum += pow(point1[i] - point2[i], 2);
    }
    return sqrt(sum);
}


void getDistance(Song *song1, Song *song2) {
    std::cout << "Distance between " << song1->getName() << " and " << song2->getName() << ": " << distance(*song1, *song2) << std::endl;
}


string getEnvVariableFromFile(string envPath, string variableName) {
    ifstream envFile(envPath);
    string line;
    while (getline(envFile, line)) {
        if (line.find(variableName) != string::npos) {
            return line.substr(line.find("=") + 1);
        }
    }
    return "";
}

int main(int argc, char* argv[]){
    // Check arguments
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <minimum song popularity> [env file path]" << endl;
        return 1;
    }

    // Get environment variables
    string envPath = argc == 3 ? argv[2] : ".env";
    string hostaddr = getEnvVariableFromFile(envPath, "POSTGRES_HOST_ADDRESS");

    char connString[1024] = {0};
    snprintf(connString, 1023, "dbname=db user=postgres password=password hostaddr=%s port=5432", hostaddr.c_str());

    printf("Connecting to database at %s\n", hostaddr.c_str());
    pqxx::connection c(connString);
    
    printf("Starting with minimum song popularity %s\n", argv[1]);

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
    
    std::vector<Song *> songs;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);


    
    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        Song *song = new Song(c[0].as<std::string>(), c[1].as<std::string>(), c[2].as<float>(), c[3].as<float>(), c[4].as<float>(), c[5].as<float>(), c[6].as<float>(), c[7].as<float>(), c[8].as<float>(), c[9].as<float>(), c[10].as<float>(), c[11].as<float>(), c[12].as<float>());
        songs.push_back(song);
        //cout << c[0].as<string>() << endl << c[1].as<string>() << endl << c[2].as<string>() << endl;
    }


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

    std::priority_queue<Song *, std::vector<Song *>, ComparePointsClosestFirst<Song>> neighbour = tree.kNearestNeighborSearch(tree.getRoot(), songs[0], 10);
    cout << "Finished searching for nearest neighbours" << endl << endl;
    
    // Print songs[0]
    printSong(songs[0]);

    while(!neighbour.empty()) {
        printSong(neighbour.top());
        getDistance(songs[0], neighbour.top());
        neighbour.pop();
    }

}


