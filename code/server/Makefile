flags = -std=c++11 -Wall
libs = -pthread 

server: main.o server.o
	g++ -o server main.o server.o $(libs) $(flags)

main.o: main.cpp
	g++ -c main.cpp $(flags)

server.o: server.cpp server.hpp
	g++ -c server.cpp $(flags)

clean:
	rm -rf server
	rm -rf *.o
