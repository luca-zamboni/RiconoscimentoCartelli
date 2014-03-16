all:
	g++ -fpermissive -std=c++11 -w provaOld.cpp -o app `pkg-config --cflags --libs opencv`

