#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class Client
{
    private:
        fd_set fds;

        int sock;
        struct sockaddr_in addr;
        struct timeval timeout;

        bool ready;
        mutable mutex mtx;
        mutable condition_variable cv;

        mutable deque < pair < string, bool > > messages;

        string lastMsg;

    public:
        Client();

        void connectToServer(string ip, int port, int timeoutSec = 5);

        void sendMSG(string data, bool force = false);
        
        void constructFineMessage(char* buffer, int size, int curMsg, int begOffset);
        void recvMSG(int size = 2048, int timeoutSec = 1);

        string getMessage() const;

        ~Client();
};
