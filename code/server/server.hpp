#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

class Server
{
    private:
        int master_sock;
        struct sockaddr_in addr;

        int max_clients;
        vector < int > client_sockets;

        fd_set readfds;

		string fineMessage;

    public:
        Server(int max_clients, int max_queue, int port);

		bool constructFineMessage(char* buffer, int size);     
 
        void checkNewConnections();
        void checkOldConnections(int size);

        void routeMSG(int size = 2048);

        ~Server();
};
