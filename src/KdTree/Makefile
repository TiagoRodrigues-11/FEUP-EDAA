CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17

SRCS = main.cpp Song.cpp KdTree.cpp KdTreeNode.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = kd

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)