#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <fstream>
#include <cmath>

#include "Song.h"

using namespace std;

void printSong(Song *song);
double distance(Song point1, Song point2);
void getDistance(Song *song1, Song *song2);
string getEnvVariableFromFile(string envPath, string variableName);
std::vector<Song *> getVectorFromDbResults(pqxx::result r);