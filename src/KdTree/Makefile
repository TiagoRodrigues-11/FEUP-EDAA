CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -pthread

target: main

Song.o: Song.cpp Song.h
	$(CXX) $(CXXFLAGS) -c Song.cpp

main.o: Song.o main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

main: clean main.o Song.o
	$(CXX) $(CXXFLAGS) main.o Song.o -o KdTree.exe

clean:
	del /f *.o *.exe