#include "node.hpp"

Node::Node(int max_clients, int max_queue, int port)
{
    this->max_clients = max_clients;
    client_sockets.resize(max_clients);
    new_client_sockets.resize(max_clients);

    messages.resize(max_clients);

    for (size_t i = 0; i < messages.size(); i++)
    {
        messages[i].resize(2, {"", false});
    }

    ready = true;

    master_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (master_sock < 0)
    {
        throw(runtime_error("ERROR::Node::Node() master_socket"));
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(master_sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        throw(runtime_error("ERROR::Node::Node() bind"));
    }

    if (listen(master_sock, max_queue) < 0)
    {
        throw(runtime_error("ERROR::Node::Node() listen"));
    }
}

void Node::checkNewConnections()
{
    if (FD_ISSET(master_sock, &readfds))
    {
        int client_socket = accept(master_sock, NULL, NULL);

        if (client_socket < 0)
        {
            throw(runtime_error("ERROR::Node::checkNewConnections() accept"));
        }

        /* welcome */
        int emptyInd = -1;
        bool newSock = true;

        for (int i = 0; i < max_clients; i++)
        {
            /* empty socket cell */
            if (!client_sockets[i] && !new_client_sockets[i] && emptyInd == -1)
            {
                emptyInd = i;
            }

            if (client_sockets[i] == client_socket)
            {
                newSock = false;
                break;
            }
        }
        
        if (newSock && emptyInd >= 0)
        {
            new_client_sockets[emptyInd] = client_socket;
            
            cout << "Join playerID: " << emptyInd << endl;
        }
    }
}

void Node::constructFineMessage(char* buffer, int size, int index)
{
	/* BEG */
	if (!buffer || size <= 0)
	{
        messages[index][0].second = false;
        messages[index][1].second = false;

        return;
    }

    string tmp(buffer, size);

    if (messages[index][0].first.empty())
    {
        size_t beg = tmp.find("BEG");
        if (beg != string::npos)
        {
            size_t end = tmp.find("END", beg + 3);
            if (end != string::npos)
            {
                messages[index][0].first = tmp.substr(beg + 3, end - beg - 3);		

                size_t beg2 = tmp.find("BEG", end + 3);
                if (beg2 != string::npos)
                {
                    size_t end2 = tmp.find("END", beg2 + 3);
                    if (end2 != string::npos)
                    {
                        messages[index][1].first = tmp.substr(beg2 + 3, end2 - beg2 - 3);
                        messages[index][0].second = true;
                        messages[index][1].second = true;

                        return;
                    }
                    else
                    {
                        messages[index][1].first = tmp.substr(beg2 + 3);
                    }
                }

                messages[index][0].second = true;
                messages[index][1].second = false;

                return;
            }
            else
            {
                messages[index][0].first = tmp.substr(beg + 3);

                messages[index][0].second = false;
                messages[index][1].second = false;

                return;
            }
        }	
    }
    else
    {
        size_t end = tmp.find("END");
        if (end != string::npos)
        {
            messages[index][0].first += tmp.substr(0, end);		

            size_t beg2 = tmp.find("BEG", end + 3);
            if (beg2 != string::npos)
            {
                size_t end2 = tmp.find("END", beg2 + 3);
                if (end2 != string::npos)
                {
                    messages[index][1].first = tmp.substr(beg2 + 3, end2 - beg2 - 3);

                    messages[index][0].second = true;
                    messages[index][1].second = true;

                    return;
                }
                else
                {
                    messages[index][1].first = tmp.substr(beg2 + 3);
                }
            }

            messages[index][0].second = true;
            messages[index][1].second = false;

            return;
        }
        else
        {
            messages[index][0].first += tmp.substr(0);		
            messages[index][0].second = false;
            messages[index][1].second = false;

            return;
        }
    }

    messages[index][0].second = false;
    messages[index][1].second = false;

    return;
    /* END */
}

void Node::checkOldConnections(int size)
{
    for (int i = 0; i < max_clients; i++)
    {
        int inputsd = client_sockets[i];

        if (FD_ISSET(inputsd, &readfds))
        {
            char* buffer = new char[size + 1];
            int bytes_read = 0;

            unique_lock < mutex > lck(mtx);
            ready = false;

            messages[i][0] = messages[i][1];
            messages[i][1] = {"", false};

            while (!messages[i][0].second)
            {	
                memset(buffer, 0, size);

                bytes_read = recv(inputsd, buffer, size, 0);

                if (bytes_read <= 0)
                {
                    break;
                }
                
                constructFineMessage(buffer, bytes_read, i);

                if (messages[i][0].second)
                {
                    /* END fix */
                    if (!messages[i][0].first.empty())
                    {
                        int s = messages[i][0].first.size() - 1;
                        while (s >= 0 && messages[i][0].first[s] != '>')
                        {
                            messages[i][0].first[s] = ' ';
                            s--;
                        }
                    }
                }
            }

            ready = true;
            cv.notify_all();

            delete[] buffer;

            /* client has disconnected */
            if (bytes_read <= 0 && !messages[i][0].second)
            {
                string message = "BEG\n<quit>" + to_string(i) + "</quit>\nEND";

                close(inputsd);
                client_sockets[i] = 0;
                new_client_sockets[i] = 0;

                messages[i][0] = {"", false};
                messages[i][1] = {"", false};

                //sendMSG(-1, message);

                cout << "Quit playerID: " << i << endl;
            }
        }
    }
}

void Node::checkActivity(int size, float timeoutSec)
{
    FD_ZERO(&readfds);

    FD_SET(master_sock, &readfds);
    int max_sd = master_sock;

    for (int i = 0; i < max_clients; i++)
    {
        int sd = client_sockets[i];

        if (sd > 0)
        {
            FD_SET(sd, &readfds);
        }

        if (sd > max_sd)
        {
            max_sd = sd;
        }
    }

    timeout.tv_sec = timeoutSec;
    timeout.tv_usec = 0;

    int activity = select(max_sd + 1, &readfds, NULL, NULL, &timeout);

    if (activity < 0 && errno != EINTR)
    {
        throw(runtime_error("ERROR::Node::checkActivity() select"));
    }

    checkNewConnections();
    checkOldConnections(size);
}

void Node::sendMSG(int to, string msg)
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    if (to > 0)
    {
        if (send(to, msg.data(), msg.size(), MSG_NOSIGNAL) < 0)
        {
            throw(runtime_error("ERROR::Node::sendMSG() send"));
        }
    }
}

vector < int > Node::getClientSockets() const
{
    return client_sockets;
}

vector < int > Node::getNewClientSockets() const
{
    return new_client_sockets;
}

bool Node::isNewClients() const
{
    for (size_t i = 0; i < new_client_sockets.size(); i++)
    {
        if (new_client_sockets[i] > 0)
        {
            return true;
        }
    }

    return false;
}

int Node::getClientSocket(size_t index) const
{
    if (!(index >= 0 && index < client_sockets.size()))
    {
        throw(runtime_error("ERROR::Node::getClientSocket() out of range"));
    }

    return client_sockets[index];
}

vector < string > Node::getMessages() const
{
    unique_lock < mutex > lck(mtx);

    while (!ready)
    {
        cv.wait(lck);
    }

    vector < string > res;

    for (size_t i = 0; i < messages.size(); i++)
    {
        res.push_back(messages[i][0].first);
    }

    return move(res);
}

void Node::newToOldClient(int index)
{
    client_sockets[index] = new_client_sockets[index];
    new_client_sockets[index] = 0;
}

Node::~Node() {}
