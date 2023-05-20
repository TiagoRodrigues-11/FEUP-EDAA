#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <queue>
#include <vector>

#include "Song.h"

using namespace std;

void printSong(Song *song);
double distance(Song &point1, Song &point2);
void getDistance(Song *song1, Song *song2);
string getEnvVariableFromFile(string envPath, string variableName);
std::vector<FullTrack *> getVectorFromDbResults(pqxx::result r);
std::vector<PartialTrack *> getPartialTrackVectorFromDbResults(pqxx::result r, vector<string> attributes);
int getAlgorithmChoice(istream &inputStream);
static inline void rtrim(std::string &s);
string getSongName(istream &inputStream);