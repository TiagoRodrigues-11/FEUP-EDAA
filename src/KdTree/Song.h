#pragma once

#include <string>
#include <map>
#include <iterator>
#include <iostream>

class Song {
    public:
        virtual std::string getName() = 0;
        virtual std::string getArtist() = 0;

        // All attributes except name and artist
        virtual size_t dimensions() = 0;
                  
        virtual double operator[] (int i) = 0;
        //virtual bool operator== (Song song) = 0;
        virtual void setDimension(int dimension, double value) = 0;
        virtual std::string getDimensionName(int dimension) = 0;
};

class FullTrack : public Song {
    public:
        FullTrack();
        FullTrack(std::string name, std::string artist, double acousticness, double danceability, double energy, double instrumentalness, double liveness, double loudness, double speechiness, double tempo, double valence, double time_signature, double mode);
        ~FullTrack();
        std::string getName();
        std::string getArtist();

        // All attributes except name and artist
        size_t dimensions();
                  
        double operator[] (int i);
        bool operator== (FullTrack song);
        void setDimension(int dimension, double value);
        std::string getDimensionName(int dimension);

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

class PartialTrack : public Song {
    public:
        PartialTrack();
        PartialTrack(std::string name, std::string artist, std::map<std::string, double> attributes);
        ~PartialTrack();
        std::string getName();
        std::string getArtist();

        // All attributes except name and artist
        size_t dimensions();
                  
        double operator[] (int i);
        bool operator== (PartialTrack song);
        void setDimension(int dimension, double value);
        void setDimension(std::string dimension, double value);
        std::string getDimensionName(int dimension);

    private:
        std::string name;
        std::string artist;
        
        std::map<std::string, double> attributes;
};

