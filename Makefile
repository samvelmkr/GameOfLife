PKGS=sdl2
CFLAGS=-Wall -ggdb -std=c++11 -pedantic `pkg-config --cflags sdl2`
LIBS=`pkg-config --libs sdl2`

main: main.cpp
	$(CXX) $(CFLAGS) -o main main.cpp $(LIBS)
