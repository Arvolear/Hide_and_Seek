#include <iostream>

#include "server.hpp"

using namespace std;

int main()
{
	Server* server = new Server(5, 1, 5040);

	while (true)
	{
		server->routeMSG();
	}

	delete server;	

	return 0;
}
