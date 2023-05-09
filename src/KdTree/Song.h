#pragma once

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
        bool operator== (Song song);
        void setDimension(int dimension, double value);

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

