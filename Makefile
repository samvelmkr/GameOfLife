CFLAGS=-Wall -ggdb -std=c++11 -pedantic
LIBS=

main: main.cpp
	$(CXX) $(CFLAGS) -o main main.cpp $(LIBS)
