#include "utils.h"

/**
 * @brief Prints a song to the console
*/
void printSong(Song *song) {
    // Name - Author
    std::cout << song->getName() << " - " << song->getArtist() << std::endl;
    // Other attributes

    for(int i = 0; i < (int)(song->dimensions()); i++){
        std::cout << song->getDimensionName(i) << ": " << (*song)[i] << std::endl;
    }

    std::cout << std::endl;
}

/**
 * @brief Calculate the distance between two songs
*/
double distance(Song &point1, Song &point2)
{
    double sum = 0;
    for (int i = 0; i < (int)(point1.dimensions()); i++)
    {
        // All this values come from the API 
        std::string dimensionName = point1.getDimensionName(i);
        if(dimensionName == "instrumentalness"){
            // Same category
            if ((point1[i] < 0.5 && point2[i] < 0.5) || (point1[i] > 0.5 && point2[i] > 0.5)) { 
                sum += pow(point1[i] - point2[i], 2);
                break;
            }
            // Different Category
            sum += pow(point1[i] - point2[i], 2) * 1.5;
        } else if (dimensionName == "liveness"){
            // Same category - Values above 0.8 means it was most likely performed live
            if((point1[i] > 0.8 && point2[i] > 0.8) || (point1[i] < 0.8 && point2[i] < 0.8)){ 
                sum += pow(point1[i] - point2[i], 2);
                break;
            } 
            // Different Category
            sum += pow(point1[i] - point2[i], 2) * 1.5;
        } else if (dimensionName == "loudness") {
            sum += pow(point1[i]/60 - point2[i]/60, 2);
        } else if (dimensionName == "speechiness"){
            // Values below 0.33 are intrumentals, between 0.33 and 0.66 are mixed, and above 0.66 are pure speech
            // Increase distance if they are not in the same category
            if ((point1[i] < 0.33 && point2[i] < 0.33) || (point1[i] > 0.66 && point2[i] > 0.66) || (point1[i] > 0.33 && point1[i] < 0.66 && point2[i] > 0.33 && point2[i] < 0.66)) {
                sum += pow(point1[i] - point2[i], 2);
                break;
            } 
            sum += pow(point1[i] - point2[i], 2) * 1.5;
        } else if (dimensionName == "tempo") {
            sum += pow(point1[i]/250 - point2[i]/250, 2);
        } else if (dimensionName == "time_signature") {
            sum += pow(point1[i]/5 - point2[i]/5, 2);
        } else if (dimensionName == "mode") {
            // If same mode, multiply by 1, else multiply by 1.5
            sum *= point1[i] == point2[i] ? 1 : 1.5; 
        } else {
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
        for(size_t i = 0; i < attributes.size(); i++){
            attValues[attributes[i]] = c[(int)(i)+2].as<float>();
        }
        PartialTrack *song = new PartialTrack(c[0].as<std::string>(), c[1].as<std::string>(), attValues);
        songs.push_back(song);
    }
    songs.shrink_to_fit();
    return songs;
}

int getAlgorithmChoice(istream &inputStream){
    int choice;
    cout << "What would you like to do?" << endl;
    cout << "1. Select a song" << endl;
    cout << "2. Range search" << endl;
    cout << "3. Exit" << endl;
    cout << "Option: ";
    inputStream >> choice;
    cout << endl;

    return choice;
}

/**
 * @brief Trim from end (in place)
*/
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

string getSongName(istream &inputStream){
    string name;
    cout << "Please enter the name of the song you would like to search for: " << endl;
    cout << "Name: ";
    std::getline(inputStream >> std::ws, name);

    // Remove trailing whitespace
    rtrim(name);

    return name;
}