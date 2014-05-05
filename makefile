all:
	g++ -fpermissive -std=c++11 -w vanishing/lsd.o vanishing/vanishing.cpp RCartels.cpp -o app `pkg-config --cflags --libs opencv`

