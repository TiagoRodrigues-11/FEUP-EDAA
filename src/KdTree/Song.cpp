#include "Song.h"


Song::Song() {
    this->name = "";
    this->artist = "";
    this->acousticness = 0;
    this->danceability = 0;
    this->energy = 0;
    this->instrumentalness = 0;
    this->liveness = 0;
    this->loudness = 0;
    this->speechiness = 0;
    this->tempo = 0;
    this->valence = 0;
    this->time_signature = 0;
    this->mode = 0;
}

Song::Song(std::string name, std::string artist, double acousticness, double danceability, double energy, double instrumentalness, double liveness, double loudness, double speechiness, double tempo, double valence, double time_signature, double mode) {
    this->name = name;
    this->artist = artist;
    this->acousticness = acousticness;
    this->danceability = danceability;
    this->energy = energy;
    this->instrumentalness = instrumentalness;
    this->liveness = liveness;
    this->loudness = loudness;
    this->speechiness = speechiness;
    this->tempo = tempo;
    this->valence = valence;
    this->time_signature = time_signature;
    this->mode = mode;
}

Song::~Song() {
}

std::string Song::getName() {
    return this->name;
}

std::string Song::getArtist() {
    return this->artist;
}

int Song::dimensions() {
    return 11;
}

double Song::operator[] (int i) {
    switch (i) {
        case 0:
            return this->acousticness;
        case 1:
            return this->danceability;
        case 2:
            return this->energy;
        case 3:
            return this->instrumentalness;
        case 4:
            return this->liveness;
        case 5:
            return this->loudness;
        case 6:
            return this->speechiness;
        case 7:
            return this->tempo;
        case 8:
            return this->valence;
        case 9:
            return this->time_signature;
        case 10:
            return this->mode;
        default:
            return 0;
    }
}