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

void Song::setDimension(int dimension, double value) {
    switch (dimension) {
        case 0:
            this->acousticness = value;
            break;
        case 1:
            this->danceability = value;
            break;
        case 2:
            this->energy = value;
            break;
        case 3: 
            this->instrumentalness = value;
            break;
        case 4:
            this->liveness = value;
            break;
        case 5:
            this->loudness = value;
            break;
        case 6:
            this->speechiness = value;
            break;
        case 7:
            this->tempo = value;
            break;
        case 8:
            this->valence = value;
            break;
        case 9:
            this->time_signature = value;
            break;
        case 10:
            this->mode = value;
            break;
        default:
            return;
    }
}

bool Song::operator== (Song song) {
    return this->name == song.getName() && this->artist == song.getArtist();
}

std::string Song::getDimensionName(int i) {
    switch (i) {
        case 0:
            return "acousticness";
        case 1:
            return "danceability";
        case 2:
            return "energy";
        case 3:
            return "instrumentalness";
        case 4:
            return "liveness";
        case 5:
            return "loudness";
        case 6:
            return "speechiness";
        case 7:
            return "tempo";
        case 8:
            return "valence";
        case 9:
            return "time_signature";
        case 10:
            return "mode";
        default:
            return "";
    }
}
