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

std::vector<Song *> getVectorFromDbResults(pqxx::result r) {
    std::vector<Song *> songs;
    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        Song *song = new Song(c[0].as<std::string>(), c[1].as<std::string>(), c[2].as<float>(), c[3].as<float>(), c[4].as<float>(), c[5].as<float>(), c[6].as<float>(), c[7].as<float>(), c[8].as<float>(), c[9].as<float>(), c[10].as<float>(), c[11].as<float>(), c[12].as<float>());
        songs.push_back(song);
    }
    songs.shrink_to_fit();
    return songs;
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

Song* selectSong(string minPopularity, string connString){
    pqxx::connection c(connString);
    
    string name;

    cout << "Please enter the name of the song you would like to search for: " << endl;
    cout << "Name: ";
    cin >> name;

    char sql[1024] = {0};
    snprintf(sql, 1023, "SELECT name, artist_names, acousticness, danceability, energy, instrumentalness, liveness, loudness, speechiness, tempo, valence, time_signature, mode " 
                      "FROM tracks_info "
                      "WHERE popularity > %s AND name ILIKE '%s'", minPopularity.c_str(), name.c_str());

    pqxx::work tx(c);

    pqxx::result r = tx.exec(sql);

    tx.commit();

    std::vector<Song *> songs = getVectorFromDbResults(r);

    if (songs.size() == 0) {
        cout << "No songs found with that name" << endl;
        return nullptr;
    } else if (songs.size() == 1) {
        cout << "Found song: " << endl;
        cout << songs[0]->getName() << " - " << songs[0]->getArtist() << endl;
        return songs[0];
    } else {
        cout << "Found " << songs.size() << " songs with that name, please select one: " << endl;
        for (int i = 0; i < songs.size(); i++) {
            cout << i << ". " << songs[i]->getName() << " - " << songs[i]->getArtist() << endl;
        }
        cout << songs.size() << ". None of the above, try again." << endl;
        cout << "Option: ";
        int choice;
        cin >> choice;
        if(choice == songs.size()) return nullptr;
        return songs[choice];
    }

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
    
    snprintf(sql, 1023, "SELECT name, artist_names, acousticness, danceability, energy, instrumentalness, liveness, loudness, speechiness, tempo, valence, time_signature, mode " 
                      "FROM tracks_info "
                      "WHERE popularity > %s ", argv[1]);

    pqxx::result r = tx.exec(sql);

    tx.commit();
    
    std::vector<Song *> songs = getVectorFromDbResults(r);

    cout << "Building KdTree with " << songs.size() << " nodes..." << endl;

    auto start = std::chrono::system_clock::now();

    // Make a KdTree
    KdTree<Song> tree = KdTree<Song>(songs);

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    
    cout << "Finished in: " << elapsed_seconds.count() << "s" << endl;

    int choice;
    cout << "What would you like to do?" << endl;
    cout << "1. Select a song" << endl;
    cout << "2. Advanced search" << endl;
    cout << "3. Exit" << endl;
    cout << "Option: ";
    cin >> choice;

    if(choice == 1){
        Song *selected = nullptr;
        while(true){
            selected = selectSong(argv[1], connString);
            if(selected != nullptr){
                cout << "Selected song: " << selected->getName() << endl;
                break;
            }
        }

        cout << "What would you like to do?" << endl;
        cout << "1. Find similar songs." << endl;
        cout << "2. Advanced search." << endl;
        cout << "3. Choose another song." << endl;
        cout << "4. Exit." << endl;
        cout << "Option: ";
        cin >> choice;

        if(choice == 1){
            std::priority_queue<Song *, std::vector<Song *>, ComparePointsClosestFirst<Song>> neighbour = tree.kNearestNeighborSearch(tree.getRoot(), selected, 10);
            cout << "Finished searching for nearest neighbours" << endl << endl;
            
            printSong(selected);

            while(!neighbour.empty()) {
                getDistance(selected, neighbour.top());
                printSong(neighbour.top());
                neighbour.pop();
            }
        } else if (choice == 2) {
            //Advanced search with starting parameters as the selected song

        } else if (choice == 3) {
            //Restart with the first choice as 1

        } else if (choice == 4) {
            return 0;
        }
    } else if (choice == 2) {
        //Advanced search

    } else if (choice == 3) {
        return 0;
    }
}


