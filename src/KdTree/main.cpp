#include <iostream>
#include "Song.h"
#include "KdTree.h"
#include <random>

int main(){ 
    // Make 10 random songs
    std::vector<Song> songs;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    for (int i = 0; i < 10; i++) {
        Song song = Song("Song " + std::to_string(i), "Artist " + std::to_string(i), dis(gen), dis
            (gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen));
        songs.push_back(song);
    }

    // Print the songs
    for (size_t i = 0; i < songs.size(); i++) {
        std::cout << songs[i].getName() << " " << songs[i].getArtist() << " " << songs[i][0] << " " << songs[i][1] << " " << songs[i][2] << " " << songs[i][3] << " " << songs[i][4] << " " << songs[i][5] << " " << songs[i][6] << " " << songs[i][7] << " " << songs[i][8] << " " << songs[i][9] << " " << songs[i][10] << " " << songs[i][11] << std::endl;
    }

    // Make a KdTree
    KdTree<Song> tree = KdTree<Song>(songs);
    

}