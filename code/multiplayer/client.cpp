#include "client.hpp"

Client::Client()
{
    sock = 0;
    buffer = nullptr;
    id = -1;
}
        
void Client::connectToServer(string ip, int port)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        throw(runtime_error("client socket"));
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    buffer = nullptr;
    id = -1;

    if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        throw(runtime_error("client connect"));
    }

    recvMSG(128);
    id = stoi(buffer);
}

void Client::sendMSG(string data)
{
    send(sock, data.data(), data.size(), 0);
}

void Client::recvMSG(int size)
{
    delete[] buffer;
    buffer = new char[size];
    memset(buffer, 0, size);

    recv(sock, buffer, size, 0);
}

char* Client::getBuffer() const
{
    return buffer;
}

int Client::getID() const
{
    return id;
}

Client::~Client()
{
    delete[] buffer;
    close(sock);
}
