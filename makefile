MAINAPP=rcartels
OBJVANI=vanishing/vani.o
OBJLSD=vanishing/lsd.o
OBJPATTUTIL=pattern_utils/patutil.o
CC=g++
OBJFLAGS=-w -c -std=c++11
FLAGS=-std=c++11 -w
OUT=app
OBJECTS=$(OBJVANI) $(OBJPATTUTIL) $(OBJLSD)
SOURCE=main.cpp RCartel.cpp RCartel.hpp
LIBS=`pkg-config --cflags --libs opencv`

all: $(MAINAPP)

$(MAINAPP): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) $(SOURCE) -o $(OUT) $(LIBS)
	
$(OBJLSD):
	$(CC) $(OBJFLAGS) vanishing/lsd/lsd.h vanishing/lsd/lsd.c -o $(OBJLSD)

$(OBJVANI):
	$(CC) $(OBJFLAGS) vanishing/vanishing.cpp -o $(OBJVANI)

$(OBJPATTUTIL):
	$(CC) $(OBJFLAGS) pattern_utils/patternutils.cpp -o $(OBJPATTUTIL)

clean:
	rm -rf $(OBJECTS) $(OUT)
