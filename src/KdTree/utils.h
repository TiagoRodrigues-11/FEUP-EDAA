#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>

#include "Song.h"

using namespace std;

void printSong(Song *song);
void getDistance(Song *song1, Song *song2);
string getEnvVariableFromFile(string envPath, string variableName);
std::vector<FullTrack *> getVectorFromDbResults(pqxx::result r);
std::vector<PartialTrack *> getPartialTrackVectorFromDbResults(pqxx::result r, vector<string> attributes);