all: rcartels

rcartels: lsd.o vani.o patutil.o
	g++ -fpermissive -std=c++11 -w lsd.o vani.o patutil.o RCartels.cpp -o app `pkg-config --cflags --libs opencv`
	rm -rf patutil.o
	
lsd.o:
	g++ -w -c vanishing/lsd/lsd.h vanishing/lsd/lsd.c

vani.o:
	g++ -w -c -std=c++11  vanishing/vanishing.cpp -o vani.o

patutil.o:
	g++ -w -c -std=c++11  pattern_utils/patternutils.cpp -o patutil.o

clean:
	rm -rf *.o app
