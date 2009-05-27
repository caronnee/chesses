CFLAGS=-O0 -g -Wall
LDFLAGS= `sdl-config --cflags --libs`

all: chess

distclean: 
		rm -f chess game.o board.o

chess: game.o board.o
		g++ $(LDFLAGS) -o chess game.o board.o

game.o: main.cpp
		g++ -c $(CFLAGS) -o game.o main.cpp 

board.o: board.h board.cpp
		g++ -c $(CFLAGS) -o board.o board.cpp 

run:
	make
	./chess
