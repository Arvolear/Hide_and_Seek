#include "client.hpp"

Client::Client()
{
    sock = 0;
    ready = true;
    lastMsg = "";
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
    arg = fcntl(sock, F_GETFL, NULL); 
    arg |= O_NONBLOCK;
    fcntl(sock, F_SETFL, arg);

    if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        if (errno == EINPROGRESS)
        {
            FD_ZERO(&fds);
            FD_SET(sock, &fds);

            timeout.tv_sec = timeoutSec;
            timeout.tv_usec = 0;

            /* timeout check */
            if (select(sock + 1, &fds, NULL, NULL, &timeout))
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
    arg = fcntl(sock, F_GETFL, NULL);
    arg &= ~O_NONBLOCK;
    fcntl(sock, F_SETFL, arg);
}

void Client::sendMSG(string data, bool force)
{
    if (data.empty() || data == "" || (data == lastMsg && !force))
    {
        return;
    }

    lastMsg = data;
    send(sock, data.data(), data.size(), 0);
}

void Client::constructFineMessage(char* buffer, int size, int curMsg, int begOffset)
{ 
    /* BEG */
    if (!buffer || size <= 0)
    {
        messages.clear();
        return;
    }

    if (begOffset >= size)
    {
        return;
    }

    string tmp(buffer, size);

    if ((int)messages.size() <= curMsg)
    {
        messages.push_back({"", false});

        size_t beg = tmp.find("BEG", begOffset);
        if (beg != string::npos)
        {
            size_t end = tmp.find("END", beg + 3);
            if (end != string::npos)
            {
                messages[curMsg].first = tmp.substr(beg + 3, end - beg - 3);
                messages[curMsg].second = true;

                constructFineMessage(buffer, size, curMsg + 1, end + 3);
                return;
            }
            else
            {
                messages[curMsg].first = tmp.substr(beg + 3);
                messages[curMsg].second = false;

                return;
            }
        }
    }
    else
    {
        size_t end = tmp.find("END", begOffset);
        if (end != string::npos)
        {
            messages[curMsg].first += tmp.substr(begOffset, end);
            messages[curMsg].second = true;

            constructFineMessage(buffer, size, curMsg + 1, end + 3);
            return;
        }
        else
        {
            messages[curMsg].first += tmp;
            messages[curMsg].second = false;

            return;
        }
    }

    return;
    /* END */
}

void Client::recvMSG(int size, int timeoutSec)
{
    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    timeout.tv_sec = timeoutSec;
    timeout.tv_usec = 0;

    if (select(sock + 1, &fds, NULL, NULL, &timeout))
    {
        char* buffer = new char[size + 1];

        unique_lock < mutex > lk(mtx);
        ready = false;

        if (!messages.empty())
        {
            messages.pop_front();
        }

        while (messages.empty() || (!messages.empty() && !messages[0].second))
        {
            memset(buffer, 0, size + 1);

            int bytes_read = recv(sock, buffer, size, MSG_DONTWAIT);

            if (bytes_read == 0)
            {
                cout << "Disconnected from the server\n";
                //break;
                exit(0);
            }
            else if (bytes_read < 0)
            {
                break;
            }

            constructFineMessage(buffer, bytes_read, 0, 0);
        }
         
        if (!messages.empty() && messages[0].second)
        {
            /* END fix */
            if (!messages[0].first.empty())
            {
                int s = messages[0].first.size() - 1;
                while (s >= 0 && messages[0].first[s] != '>')
                {
                    messages[0].first[s] = ' ';
                    s--;
                }
            }
        }

        ready = true;
        cv.notify_all();

        delete[] buffer;
    }
    else
    {
        unique_lock < mutex > lk(mtx);
        ready = false;

        messages.clear();

        ready = true;
        cv.notify_all();
    }
}

string Client::getMessage() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    return !messages.empty() && messages[0].second ? messages[0].first : "";
}

Client::~Client()
{
    close(sock);
}
