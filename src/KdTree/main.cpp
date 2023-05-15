#include <iostream>
#include <fstream>
#include "Song.h"
#include "KdTree.hpp"
#include <random>
#include <chrono>
#include <queue>
#include <limits>
#include <pqxx/pqxx>
#include "utils.h"

using namespace std;

KdTree<Song> createKdTree(string minPopularity, string connString){
    pqxx::connection c(connString);
    
    cout << "Starting with minimum song popularity " << minPopularity << endl;

    pqxx::work tx(c);

    char sql[1024] = {0};
    
    snprintf(sql, 1023, "SELECT name, artist_names, acousticness, danceability, energy, instrumentalness, liveness, loudness, speechiness, tempo, valence, time_signature, mode " 
                      "FROM tracks_info "
                      "WHERE popularity > %s ", minPopularity.c_str());

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

    return tree;
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
        for (size_t i = 0; i < songs.size(); i++) {
            cout << i << ". " << songs[i]->getName() << " - " << songs[i]->getArtist() << endl;
        }
        cout << songs.size() << ". None of the above, try again." << endl;
        cout << "Option: ";
        size_t choice;
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

    KdTree<Song> tree = createKdTree(argv[1], connString);

    int choice;
    cout << "What would you like to do?" << endl;
    cout << "1. Select a song" << endl;
    cout << "2. Advanced search" << endl;
    cout << "3. Exit" << endl;
    cout << "Option: ";
    cin >> choice;

    bool exit = false;
    while(!exit){
        if(choice == 1){
            bool choice1 = true;
            while(choice1){
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

                    choice1 = false;
                    exit = true;
                } else if (choice == 2) {
                    //Advanced search with starting parameters as the selected song

                } else if (choice == 3) {
                    continue;
                } else if (choice == 4) {
                    choice1 = false;
                    exit = true;
                }
            }
        } else if (choice == 2) {
            Song min = Song();
            Song max = Song();
            for (int i = 0; i < min.dimensions(); i++) {
                double minValue, maxValue;
                cout << "Please enter the minimum value for " << min.getDimensionName(i) << ": ";
                cin >> minValue;
                cout << "Please enter the maximum value for " << min.getDimensionName(i) << ": ";
                cin >> maxValue;
                min.setDimension(i, minValue);
                max.setDimension(i, maxValue);
            }
            
            std::map<int, std::pair<double, double>> kdTreeRanges;
            for(int i = 0; i < min.dimensions(); i++){
                kdTreeRanges[i] = std::pair<double, double>(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
            }
            std::vector<Song*> points = tree.rangeSearch(tree.getRoot(), &min, &max, kdTreeRanges, 0);

            cout << "Found " << points.size() << " songs matching your search" << endl;
            for(size_t i = 0; i < points.size(); i++){
                printSong(points[i]);
            }

            return 0;
        } else if (choice == 3) {
            exit = true;
        }
    }

    
}


