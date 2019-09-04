#include "client.hpp"

Client::Client()
{
    sock = 0;
    message = "";
    ready = true;
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
}

void Client::sendMSG(string data)
{
    send(sock, data.data(), data.size(), 0);
}

bool Client::constructFineMessage(char* buffer, int size)
{
	/* BEG */
	if (!buffer || size <= 0)
	{
		return false;
	}

	string tmp(buffer, size);

	if (message.empty())
	{
		size_t beg = tmp.find("BEG");
		if (beg != string::npos)
		{
			size_t end = tmp.find("END", beg + 3);
			if (end != string::npos)
            {
                message = tmp.substr(beg + 3, end - beg - 3);		
                return true;
            }
            else
            {
                message = tmp.substr(beg + 3);
                return false;
            }
        }	
    }
    else
    {
        size_t end = tmp.find("END");
        if (end != string::npos)
        {
            message += tmp.substr(0, end);		
            return true;
        }
        else
        {
            message += tmp.substr(0);		
            return false;
        }
    }

    return false;
    /* END */
}

void Client::recvMSG(int size, int timeoutSec)
{
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    timeout.tv_sec = timeoutSec;
    timeout.tv_usec = 0;

    if (select(sock + 1, &readfds, NULL, NULL, &timeout))
    {
        char* buffer = new char[size + 1];
        
        unique_lock < mutex > lk(mtx);
        ready = false;

        message = "";

        while (true)
        {
            memset(buffer, 0, size);

            int bytes_read = recv(sock, buffer, size, 0);

            if (bytes_read <= 0)
            {
                break;
            }

            if (constructFineMessage(buffer, bytes_read))
            {
                /* END fix */
                if (!message.empty())
                {
                    int s = message.size() - 1;
                    while (s >= 0 && message[s] != '>')
                    {
                        message[s] = ' ';
                        s--;
                    }
                }

                break;
            }
        }

        ready = true;
        cv.notify_all();

        delete[] buffer;
    }
}

string Client::getMessage() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    return message;
}

Client::~Client()
{
    close(sock);
}
