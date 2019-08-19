#include "client.hpp"

Client::Client()
{
    sock = 0;
    buffer = nullptr;
}
        
void Client::connectToServer(string ip, int port, int timeoutSec)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        throw(runtime_error("ERROR::Client::connectToServer() socket creation"));
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
   
    int arg;

    /* set nonblocking mode */
    if ((arg = fcntl(sock, F_GETFL, NULL)) < 0)
    {
        throw(runtime_error("ERROR::Client::connectToServer() fcntl first get"));
    }
    
    arg |= O_NONBLOCK;
    if (fcntl(sock, F_SETFL, arg) < 0)
    {
        throw(runtime_error("ERROR::Client::connectToServer() fcntl first set"));
    }

    buffer = nullptr;

    if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        if (errno == EINPROGRESS)
        {
            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);

            timeout.tv_sec = timeoutSec;
            timeout.tv_usec = 0;

            /* timeout check */
            if (select(sock + 1, &readfds, NULL, NULL, &timeout))
            {
                int so_error;
                socklen_t len = sizeof(so_error);

                if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len) < 0)
                {
                    throw(runtime_error("ERROR::Client::connectToServer() getsockopt")); 
                }

                if (!so_error)
                {
                    cout << "Connected to " << ip << ":" << port << endl;
                }
            }
            else
            {
                throw(runtime_error("ERROR::Client::connectToServer() timeout connect"));
            }
        }
        else
        {
            throw(runtime_error("ERROR::Client::connectToServer() failed to connect")); 
        }
    }

    /* set blocking mode */
    if ((arg = fcntl(sock, F_GETFL, NULL)) < 0)
    {
        throw(runtime_error("ERROR::Client::connectToServer() fcntl second get"));
    }

    arg &= ~O_NONBLOCK;
    if (fcntl(sock, F_SETFL, arg) < 0)
    {
        throw(runtime_error("ERROR::Client::connectToServer() fcntl second set"));
    }

    recvMSG(128);
}

void Client::sendMSG(string data)
{
    send(sock, data.data(), data.size(), 0);
}

void Client::recvMSG(int size, int timeoutSec)
{
    delete[] buffer;
    buffer = nullptr;

    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    timeout.tv_sec = timeoutSec;
    timeout.tv_usec = 0;

    if (select(sock + 1, &readfds, NULL, NULL, &timeout))
    {
        buffer = new char[size];
        memset(buffer, 0, size);

        recv(sock, buffer, size, 0);
    }
}

char* Client::getBuffer() const
{
    return buffer;
}

Client::~Client()
{
    delete[] buffer;
    close(sock);
}
