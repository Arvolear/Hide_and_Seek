#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include <iostream>
#include <string>
#include <thread>

using namespace std;

class Client
{
    private:
        int sock;
        struct sockaddr_in addr;

        char* buffer;

        int id;

    public:
        Client(string ip, int port);

        void sendMSG(string data);
        void recvMSG(int size = 2048);

        char* getBuffer() const;
        int getID() const;

        ~Client();

};
