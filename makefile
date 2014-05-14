RICCARTELS=rcartels
OBJVANI=vanishing/vani.o
OBJLSD=vanishing/lsd.o
OBJPATTUTIL=pattern_utils/patutil.o
CC=g++
OBJFLAGS=-w -c -std=c++11
FLAGS=-fpermissive -std=c++11 -w
OUT=app
OBJECTS=$(OBJVANI) $(OBJPATTUTIL) $(OBJLSD)
SOURCE=RCartels.cpp
LIBS=`pkg-config --cflags --libs opencv`



all: $(RICCARTELS)

$(RICCARTELS): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) $(SOURCE) -o $(OUT) $(LIBS)
	rm -rf pattern_utils/patutil.o
	
$(OBJLSD):
	$(CC) $(OBJFLAGS) vanishing/lsd/lsd.h vanishing/lsd/lsd.c -o $(OBJLSD)

$(OBJVANI):
	$(CC) $(OBJFLAGS) vanishing/vanishing.cpp -o $(OBJVANI)

$(OBJPATTUTIL):
	$(CC) $(OBJFLAGS) pattern_utils/patternutils.cpp -o $(OBJPATTUTIL)

clean:
	rm -rf $(OBJECTS) $(OUT)
