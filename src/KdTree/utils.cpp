#include "utils.h"

void printSong(Song *song) {
    // Name - Author
    std::cout << song->getName() << " - " << song->getArtist() << std::endl;
    // Other attributes
    std::cout << "Acousticness: " << (*song)[0] << std::endl;
    std::cout << "Danceability: " << (*song)[1] << std::endl;
    std::cout << "Energy: " <<  (*song)[2] << std::endl;
    std::cout << "Instrumentalness: " <<  (*song)[3] << std::endl;
    std::cout << "Liveness: " <<  (*song)[4] << std::endl;
    std::cout << "Loudness: " <<  (*song)[5] << std::endl;
    std::cout << "Speechiness: " <<  (*song)[6] << std::endl;
    std::cout << "Tempo: " <<  (*song)[7] << std::endl;
    std::cout << "Valence: " <<  (*song)[8] << std::endl;
    std::cout << "Time Signature: " <<  (*song)[9] << std::endl;
    std::cout << "Mode: " << (*song)[10] << std::endl;
    std::cout << std::endl;
}

double distance(Song point1, Song point2)
{
    double sum = 0;
    for (int i = 0; i < point1.dimensions(); i++)
    {
        sum += pow(point1[i] - point2[i], 2);
    }
    return sqrt(sum);
}

void getDistance(Song *song1, Song *song2) {
    std::cout << "Distance between " << song1->getName() << " and " << song2->getName() << ": " << distance(*song1, *song2) << std::endl;
}

string getEnvVariableFromFile(string envPath, string variableName) {
    ifstream envFile(envPath);
    string line;
    while (getline(envFile, line)) {
        if (line.find(variableName) != string::npos) {
            return line.substr(line.find("=") + 1);
        }
    }
    return "";
}

std::vector<Song *> getVectorFromDbResults(pqxx::result r) {
    std::vector<Song *> songs;
    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        Song *song = new Song(c[0].as<std::string>(), c[1].as<std::string>(), c[2].as<float>(), c[3].as<float>(), c[4].as<float>(), c[5].as<float>(), c[6].as<float>(), c[7].as<float>(), c[8].as<float>(), c[9].as<float>(), c[10].as<float>(), c[11].as<float>(), c[12].as<float>());
        songs.push_back(song);
    }
    songs.shrink_to_fit();
    return songs;
}