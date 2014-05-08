all: rcartels

rcartels: lsd.o vani.o
	g++ -fpermissive -std=c++11 -w lsd.o vani.o RCartels.cpp -o app `pkg-config --cflags --libs opencv`
	
lsd.o:
	g++ -w -c vanishing/lsd/lsd.h vanishing/lsd/lsd.c

vani.o:
	g++ -c -std=c++11 -w vanishing/vanishing.cpp -o vani.o `pkg-config --cflags --libs opencv`

clean:
	rm -rf *o app
