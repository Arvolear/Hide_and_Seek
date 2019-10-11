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
#include <deque>
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
        vector < int > old_client_sockets;

        fd_set readfds;

		mutable vector < deque < pair < string, bool > > > messages;
        vector < string > lastMsgs; 
        
        bool ready;
        mutable mutex mtx;
        mutable condition_variable cv;
		
        void constructFineMessage(char* buffer, int size, int index, int curMsg, int begOffset);
 
        void checkNewConnections();
        void checkConnections(int size);

    public:
        Node(int max_clients, int max_queue, int port);

        void checkActivity(int size = 2048, float timeoutSec = 1);
        void sendMSG(int to, string msg, bool force = false);

        bool isNewClients() const;
        bool isOldClients() const;

        vector < int > getClientSockets() const;
        vector < int > getNewClientSockets() const;
        vector < int > getOldClientSockets() const;
        int getClientSocket(size_t index) const;
        vector < string > getMessages() const;

        void newToClient(int index);
        void oldToNothing(int index);

        ~Node();
};
