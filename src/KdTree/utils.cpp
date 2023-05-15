#include "utils.h"

/**
 * @brief Prints a song to the console
*/
void printSong(Song *song) {
    // Name - Author
    std::cout << song->getName() << " - " << song->getArtist() << std::endl;
    // Other attributes

    for(int i = 0; i < song->dimensions(); i++){
        std::cout << song->getDimensionName(i) << ": " << (*song)[i] << std::endl;
    }

    std::cout << std::endl;

    /* std::cout << "Acousticness: " << (*song)[0] << std::endl;
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
    std::cout << std::endl; */
}

double distance(Song &point1, Song &point2)
{
    double sum = 0;
    for (int i = 0; i < point1.dimensions(); i++)
    {
        if (i == 3) { // instrumentalness
            if ((point1[i] < 0.5 && point2[i] < 0.5) || (point1[i] > 0.5 && point2[i] > 0.5)) { //same category
                sum += pow(point1[i] - point2[i], 2);
            } else { //different category
                sum += pow(point1[i] - point2[i], 2) * 1.5;
            }
        }   
        else if(i == 4) { //liveness - values above 0.8 means it was most likely performed live
            if((point1[i] > 0.8 && point2[i] > 0.8) || (point1[i] < 0.8 && point2[i] < 0.8)){ //same category
                sum += pow(point1[i] - point2[i], 2);
            } else { //different category
                sum += pow(point1[i] - point2[i], 2) * 1.5;
            }
        } else if (i == 5) { //loudness
            sum += pow(point1[i]/60 - point2[i]/60, 2);
        }
        else if (i == 6) { //speechiness
            // values below 0.33 are intrumentals, between 0.33 and 0.66 are mixed, and above 0.66 are pure speech
            // increase distance if they are not in the same category
            if ((point1[i] < 0.33 && point2[i] < 0.33) || (point1[i] > 0.66 && point2[i] > 0.66) || (point1[i] > 0.33 && point1[i] < 0.66 && point2[i] > 0.33 && point2[i] < 0.66)) {
                sum += pow(point1[i] - point2[i], 2);
            } else {
                sum += pow(point1[i] - point2[i], 2) * 1.5;
            }
        }        
        else if (i == 7) { //tempo
            sum += pow(point1[i]/250 - point2[i]/250, 2);
        } else if (i == 9) { //time signature
            sum += pow(point1[i]/5 - point2[i]/5, 2);
        } else if (i = 10) { //mode
            sum *= point1[i] == point2[i] ? 1 : 1.5; // if same mode, multiply by 1, else multiply by 1.5 (change weights later)
        }
        else {
            sum += pow(point1[i] - point2[i], 2);
        }
    }
    return sqrt(sum);
}

/**
 * @brief Calculate and print the distance between two songs
*/
void getDistance(Song *song1, Song *song2) {
    std::cout << "Distance between " << song1->getName() << " and " << song2->getName() << ": " << distance(*song1, *song2) << std::endl;
}

/**
 * @brief Get the Environment Variable From File object
*/
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


/**
 * @brief Create a vector containing songs from the database results
*/
std::vector<FullTrack *> getVectorFromDbResults(pqxx::result r) {
    std::vector<FullTrack *> songs;
    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        FullTrack *song = new FullTrack(c[0].as<std::string>(), c[1].as<std::string>(), c[2].as<float>(), c[3].as<float>(), c[4].as<float>(), c[5].as<float>(), c[6].as<float>(), c[7].as<float>(), c[8].as<float>(), c[9].as<float>(), c[10].as<float>(), c[11].as<float>(), c[12].as<float>());
        songs.push_back(song);
    }
    songs.shrink_to_fit();
    return songs;
}

vector<PartialTrack *> getPartialTrackVectorFromDbResults(pqxx::result r, vector<string> attributes){
    std::vector<PartialTrack *> songs;
    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        map<string, double> attValues;
        for(int i = 0; i < attributes.size(); i++){
            attValues[attributes[i]] = c[i+2].as<float>();
        }
        PartialTrack *song = new PartialTrack(c[0].as<std::string>(), c[1].as<std::string>(), attValues);
        songs.push_back(song);
    }
    songs.shrink_to_fit();
    return songs;
}