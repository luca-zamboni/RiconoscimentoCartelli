all:
	g++ -w -c vanishing/lsd/lsd.c
	g++ -w -c vanishing/vanishing.cpp `pkg-config --cflags --libs opencv`
	g++ -fpermissive -w lsd.o vanishing.o RCartels.cpp -o app `pkg-config --cflags --libs opencv`

