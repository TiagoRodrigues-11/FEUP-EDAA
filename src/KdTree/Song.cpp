#include "Song.h"

FullTrack::FullTrack() {
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
    this->timeSignature = 0;
    this->mode = 0;
}

FullTrack::FullTrack(std::string name, std::string artist, double acousticness, double danceability, double energy, double instrumentalness, double liveness, double loudness, double speechiness, double tempo, double valence, double timeSignature, double mode) {
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
    this->timeSignature = timeSignature;
    this->mode = mode;
}

FullTrack::~FullTrack(){}

std::string FullTrack::getName() {
    return this->name;
}

std::string FullTrack::getArtist() {
    return this->artist;
}

size_t FullTrack::dimensions() {
    return 11;
}

double FullTrack::operator[] (int i) {
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
            return this->timeSignature;
        case 10:
            return this->mode;
        default:
            return 0;
    }
}

void FullTrack::setDimension(int dimension, double value) {
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
            this->timeSignature = value;
            break;
        case 10:
            this->mode = value;
            break;
        default:
            return;
    }
}

bool FullTrack::operator== (FullTrack song) {
    return this->name == song.getName() && this->artist == song.getArtist();
}

std::string FullTrack::getDimensionName(int i) {
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

PartialTrack::PartialTrack(){
    this->artist = "";
    this->name = "";
    this->attributes = std::map<std::string, double>();
}

PartialTrack::PartialTrack(std::string name, std::string artist, std::map<std::string, double> attributes){
    this->name = name;
    this->artist = artist;
    this->attributes = attributes;
}

PartialTrack::~PartialTrack(){}

std::string PartialTrack::getName(){
    return this->name;
}

std::string PartialTrack::getArtist(){
    return this->artist;
}

size_t PartialTrack::dimensions(){
    return this->attributes.size();
}

double PartialTrack::operator[] (int i){
    auto it = (this->attributes).begin();
    std::advance(it, i);
    return it->second;
}

void PartialTrack::setDimension(int dimension, double value){
    auto it = (this->attributes).begin();
    std::advance(it, dimension);
    it->second = value;
}

void PartialTrack::setDimension(std::string dimension, double value){
    (this->attributes)[dimension] = value;
}

std::string PartialTrack::getDimensionName(int dimension){
    auto it = (this->attributes).begin();
    std::advance(it, dimension);
    return it->first;
}
