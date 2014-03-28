all:
	g++ -fpermissive -std=c++11 -w RCartels.cpp -o app `pkg-config --cflags --libs opencv`

