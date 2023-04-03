#ifndef SONG_H
#define SONG_H

#include <string>

class Song {

    public:
        Song();
        Song(std::string name, std::string artist, double acousticness, double danceability, double energy, double instrumentalness, double liveness, double loudness, double speechiness, double tempo, double valence, double time_signature, double mode);
        ~Song();
        std::string getName();
        std::string getArtist();

        // All attributes except name and artist
        int dimensions();           
        double operator[] (int i);

    private:

        std::string name;
        std::string artist;
        
        double acousticness;
        double danceability;
        double energy;
        double instrumentalness;
        double liveness;
        double loudness;
        double speechiness;
        double tempo;
        double valence;
        double time_signature;
        double mode;
};

#endif