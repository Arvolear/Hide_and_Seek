#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;

class Node
{
    private:
        int master_sock;
        struct sockaddr_in addr;
        struct timeval timeout;

        int max_clients;
        vector < int > client_sockets;
        vector < int > new_client_sockets;

        fd_set readfds;

		vector < string > messages;
        
        bool ready;
        mutable mutex mtx;
        mutable condition_variable cv;

    public:
        Node(int max_clients, int max_queue, int port);

		bool constructFineMessage(char* buffer, int size, int index);     
 
        void checkNewConnections();
        void checkOldConnections(int size);

        void checkActivity(int size = 2048, float timeoutSec = 1);
        void sendMSG(int to, string msg);

        vector < int > getClientSockets() const;
        vector < int > getNewClientSockets() const;
        bool isNewClients() const;
        int getClientSocket(size_t index) const;
        vector < string > getMessages() const;
        void newToOldClient(int index);

        ~Node();
};
