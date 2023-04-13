#include <iostream>
#include "Song.h"
#include "KdTree.hpp"
#include <random>
#include <chrono>

using namespace std;

int main(){ 
    // Make 10 random songs
    std::vector<Song> songs;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    for (int i = 0; i < 8750000; i++) {
        Song song = Song("Song " + std::to_string(i), "Artist " + std::to_string(i), dis(gen), dis
            (gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen));
        songs.push_back(song);
    }

    songs.shrink_to_fit();
    // Print the songs
    /*for (size_t i = 0; i < songs.size(); i++) {
        std::cout << songs[i].getName() << " " << songs[i].getArtist() << " " << songs[i][0] << " " << songs[i][1] << " " << songs[i][2] << " " << songs[i][3] << " " << songs[i][4] << " " << songs[i][5] << " " << songs[i][6] << " " << songs[i][7] << " " << songs[i][8] << " " << songs[i][9] << " " << songs[i][10] << " " << songs[i][11] << std::endl;
    }*/

    cout << "Building KdTree with " << songs.size() << " nodes..." << endl;

    auto start = std::chrono::system_clock::now();

    // Make a KdTree
    KdTree<Song> tree = KdTree<Song>(songs);

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    
    cout << "Finished in: " << elapsed_seconds.count() << "s" << endl;
}