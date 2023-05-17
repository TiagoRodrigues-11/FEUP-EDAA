#include <iostream>
#include <fstream>
#include "Song.h"
#include "KdTree.hpp"
#include <random>
#include <chrono>
#include <queue>
#include <limits>
#include <algorithm>
#include <pqxx/pqxx>
#include "utils.h"

using namespace std;

KdTree<FullTrack> createKdTree(string minPopularity, string connString){
    pqxx::connection c(connString);
    
    cout << "Starting with minimum song popularity " << minPopularity << endl;

    pqxx::work tx(c);

    char sql[1024] = {0};
    
    snprintf(sql, 1023, "SELECT name, artist_names, acousticness, danceability, energy, instrumentalness, liveness, loudness, speechiness, tempo, valence, time_signature, mode " 
                      "FROM tracks_info "
                      "WHERE popularity >= %s ", minPopularity.c_str());

    pqxx::result r = tx.exec(sql);

    tx.commit();
    
    std::vector<FullTrack *> songs = getVectorFromDbResults(r);

    cout << "Building KdTree with " << songs.size() << " nodes..." << endl << endl;

    auto start = std::chrono::system_clock::now();

    // Make a KdTree
    KdTree<FullTrack> tree = KdTree<FullTrack>(songs);

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    
    cout << "Finished in: " << elapsed_seconds.count() << "s" << endl << endl;

    return tree;
}

KdTree<PartialTrack> createKdTree(string minPopularity, string connString, vector<string> attributes){
    pqxx::connection c(connString);
    
    cout << "Starting with minimum song popularity " << minPopularity << endl;

    char sql[1024] = {0};

    string inst = "SELECT name, artist_names";

    for(size_t i = 0; i < attributes.size(); i++){
        inst += (", " + attributes[i]);
    }
    
    snprintf(sql, 1023,"%s FROM tracks_info WHERE popularity >= %s ", inst.c_str(), minPopularity.c_str());

    cout << sql << endl;

    pqxx::work tx(c);

    pqxx::result r = tx.exec(sql);

    tx.commit();
    
    std::vector<PartialTrack *> songs = getPartialTrackVectorFromDbResults(r, attributes);

    cout << "Building KdTree with " << songs.size() << " nodes..." << endl << endl;

    auto start = std::chrono::system_clock::now();

    // Make a KdTree
    KdTree<PartialTrack> tree = KdTree<PartialTrack>(songs);

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    
    cout << "Finished in: " << elapsed_seconds.count() << "s" << endl << endl;

    return tree;
}

FullTrack* selectSong(string minPopularity, string connString){
    pqxx::connection c(connString);
    
    string name;

    cout << "Please enter the name of the song you would like to search for: " << endl;
    cout << "Name: ";
    std::getline(std::cin >> std::ws, name);

    char sql[1024] = {0};
    snprintf(sql, 1023, "SELECT name, artist_names, acousticness, danceability, energy, instrumentalness, liveness, loudness, speechiness, tempo, valence, time_signature, mode " 
                      "FROM tracks_info "
                      "WHERE popularity >= %s AND name ILIKE '%s'", minPopularity.c_str(), name.c_str());

    pqxx::work tx(c);

    pqxx::result r = tx.exec(sql);

    tx.commit();

    std::vector<FullTrack *> songs = getVectorFromDbResults(r);

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

void useFullTrackKdTree(string minPopularity, string connString){
    KdTree<FullTrack> tree = createKdTree(minPopularity, connString);

    int choice;
    cout << "What would you like to do?" << endl;
    cout << "1. Select a song" << endl;
    cout << "2. Range search" << endl;
    cout << "3. Exit" << endl;
    cout << "Option: ";
    cin >> choice;

    bool exit = false;
    while(!exit){
        if(choice == 1){
            while(true){
                FullTrack *selected = nullptr;
                while(true){
                    selected = selectSong(minPopularity, connString);
                    if(selected != nullptr){
                        cout << "Selected song: " << selected->getName() << endl;
                        break;
                    }
                }

                cout << "What would you like to do?" << endl;
                cout << "1. Find similar songs." << endl;
                cout << "2. Choose another song." << endl;
                cout << "3. Exit." << endl;
                cout << "Option: ";
                cin >> choice;

                if(choice == 1){
                    std::priority_queue<FullTrack *, std::vector<FullTrack *>, ComparePointsClosestFirst<FullTrack>> neighbour = tree.kNearestNeighborSearch(tree.getRoot(), selected, 10);
                    cout << "Finished searching for nearest neighbours" << endl << endl;
                    
                    printSong(selected);

                    while(!neighbour.empty()) {
                        getDistance(selected, neighbour.top());
                        printSong(neighbour.top());
                        neighbour.pop();
                    }

                    exit = true;
                    break;
                } else if (choice == 2) {
                    continue;
                } else if (choice == 3) {
                    exit = true;
                    break;
                }
            }
        } else if (choice == 2) {
            FullTrack min = FullTrack();
            FullTrack max = FullTrack();

            int dimesions = (int)min.dimensions();
            for (int i = 0; i < dimesions; i++) {
                double minValue, maxValue;
                cout << "Please enter the minimum value for " << min.getDimensionName(i) << ": ";
                cin >> minValue;
                cout << "Please enter the maximum value for " << min.getDimensionName(i) << ": ";
                cin >> maxValue;
                min.setDimension(i, minValue);
                max.setDimension(i, maxValue);
            }
            
            std::map<int, std::pair<double, double>> kdTreeRanges;
            for(int i = 0; i < dimesions; i++){
                kdTreeRanges[i] = std::pair<double, double>(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
            }
            std::vector<FullTrack*> points = tree.rangeSearch(tree.getRoot(), &min, &max, kdTreeRanges, 0);

            cout << "Found " << points.size() << " songs matching your search" << endl;
            int min_size = std::min(20, (int)points.size());
            for(int i = 0; i < min_size; i++){
                printSong(points[(size_t)i]);
            }

            return;
        } else if (choice == 3) {
            exit = true;
        }
    }
}

PartialTrack* selectSong(string minPopularity, string connString, vector<string> attributes){
    pqxx::connection c(connString);
    
    string name;

    cout << "Please enter the name of the song you would like to search for: " << endl;
    cout << "Name: ";
    std::getline(std::cin >> std::ws, name);

    string inst = "SELECT name, artist_names";

    for(size_t i = 0; i < attributes.size(); i++){
        inst += (", " + attributes[i]);
    }
    
    char sql[1024] = {0};
    snprintf(sql, 1023,"%s FROM tracks_info WHERE popularity >= %s AND name ILIKE '%s'", inst.c_str(), minPopularity.c_str(), name.c_str());

    pqxx::work tx(c);

    pqxx::result r = tx.exec(sql);

    tx.commit();

    std::vector<PartialTrack *> songs = getPartialTrackVectorFromDbResults(r, attributes);

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

void usePartialTrackKdTree(string minPopularity, string connString){
    std::vector<std::string> attributes = std::vector<std::string>();
    
    FullTrack min = FullTrack();
    int dimensions = (int)min.dimensions();
    for (int i = 0; i < dimensions; i++) {
        string dim_choice;
        cout << "Use " << min.getDimensionName(i) << "(y/n): ";
        cin >> dim_choice;

        if(dim_choice == "y") attributes.push_back(min.getDimensionName(i));
    }

    KdTree<PartialTrack> tree = createKdTree(minPopularity, connString, attributes);

    int choice;
    cout << "What would you like to do?" << endl;
    cout << "1. Select a song" << endl;
    cout << "2. Range search" << endl;
    cout << "3. Exit" << endl;
    cout << "Option: ";
    cin >> choice;

    bool exit = false;
    while(!exit){
        if(choice == 1){
            while(true){
                PartialTrack *selected = nullptr;
                while(true){
                    selected = selectSong(minPopularity, connString, attributes);
                    if(selected != nullptr){
                        cout << "Selected song: " << selected->getName() << endl;
                        break;
                    }
                }

                cout << "What would you like to do?" << endl;
                cout << "1. Find similar songs." << endl;
                cout << "2. Choose another song." << endl;
                cout << "3. Exit." << endl;
                cout << "Option: ";
                cin >> choice;

                if(choice == 1){
                    std::priority_queue<PartialTrack *, std::vector<PartialTrack *>, ComparePointsClosestFirst<PartialTrack>> neighbour = tree.kNearestNeighborSearch(tree.getRoot(), selected, 10);
                    cout << "Finished searching for nearest neighbours" << endl << endl;
                    
                    printSong(selected);

                    while(!neighbour.empty()) {
                        getDistance(selected, neighbour.top());
                        printSong(neighbour.top());
                        neighbour.pop();
                    }

                    exit = true;
                    break;
                } else if (choice == 2) {
                    continue;
                } else if (choice == 3) {
                    exit = true;
                    break;
                }
            }
        } else if (choice == 2) {
            map<string, double> dim_attributes = map<string, double>();
            
            for(size_t i = 0; i < attributes.size(); i++){
                dim_attributes[attributes[i]] = 0;
            }

            PartialTrack min = PartialTrack("", "", dim_attributes);
            PartialTrack max = PartialTrack("", "", dim_attributes);

            int dimensions = (int)dim_attributes.size();
            
            for (int i = 0; i < dimensions; i++) {
                double minValue, maxValue;
                string dimensionName = min.getDimensionName(i);
                cout << "Please enter the minimum value for " << dimensionName << ": ";
                cin >> minValue;
                cout << "Please enter the maximum value for " << dimensionName << ": ";
                cin >> maxValue;

                min.setDimension(i, minValue);
                max.setDimension(i, maxValue);
            }
            printSong(&min);
            printSong(&max);
            
            std::map<int, std::pair<double, double>> kdTreeRanges;

            for(int i = 0; i < dimensions; i++){
                kdTreeRanges[i] = std::pair<double, double>(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
            }
            
            std::vector<PartialTrack*> points = tree.rangeSearch(tree.getRoot(), &min, &max, kdTreeRanges, 0);

            cout << "Found " << points.size() << " songs matching your search" << endl;
            int min_size = std::min(20, (int)points.size());
            for(int i = 0; i < min_size; i++){
                printSong(points[(size_t)i]);
            }

            return;
        } else if (choice == 3) {
            exit = true;
        }
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

    int choice;
    cout << "Would you like to create the tree regularly or customize the dimensions?" << endl;
    cout << "1. Standard" << endl;
    cout << "2. Customized" << endl;
    cout << "Option: ";
    cin >> choice;

    switch(choice) {
        case 1:
            useFullTrackKdTree(argv[1], connString);
            break;
        case 2:
            usePartialTrackKdTree(argv[1], connString);
            break;
        default:
            cout << "Error" << endl;
            return 0;
    }
}


