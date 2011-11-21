CC=g++
CFLAGS=-c -Wall -DLINUX
LDFLAGS=`sdl-config --cflags --libs` -framework Cocoa
SOURCES=Main.cpp Game.cpp Board.cpp IO.cpp Pieces.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tetris

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS) $(EXECUTABLE)
