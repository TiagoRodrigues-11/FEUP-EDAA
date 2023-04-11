CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -pthread

target: main

KdTree.o: KdTree.cpp KdTree.h
	$(CXX) $(CXXFLAGS) -c KdTree.cpp

Song.o: Song.cpp Song.h
	$(CXX) $(CXXFLAGS) -c Song.cpp

main.o: KdTree.o Song.o main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

main: main.o KdTree.o Song.o
	$(CXX) $(CXXFLAGS) main.o KdTree.o Song.o -o KdTree.exe

clean:
	rm -f *.o