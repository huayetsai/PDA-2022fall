all: Lab2

CXX = g++ -std=c++11

LINKFLAGS = -Wall

Lab2: main.o b_star_tree.o
	$(CXX) $(LINKFLAGS) -o Lab2 main.o b_star_tree.o

main.o: main.cpp
	$(CXX) $(LINKFLAGS) -c main.cpp 

b_star_tree.o: b_star_tree.cpp b_star_tree.h
	$(CXX) $(LINKFLAGS) -c b_star_tree.cpp

clean:
	rm -rf *.o Lab2
