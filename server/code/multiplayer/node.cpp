#include "node.hpp"

Node::Node(int max_clients, int max_queue, int port)
{
    this->max_clients = max_clients;
    client_sockets.resize(max_clients);
    new_client_sockets.resize(max_clients);
    old_client_sockets.resize(max_clients);

    messages.resize(max_clients);
    lastMsgs.resize(max_clients, "");

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
        }
    }
}

void Node::constructFineMessage(char* buffer, int size, int index, int curMsg, int begOffset)
{
	/* BEG */
	if (!buffer || size <= 0)
	{
        messages[index].clear();

        return;
    }

    if (begOffset >= size)
    {
        return;
    }

    string tmp(buffer, size);

    if ((int)messages[index].size() <= curMsg)
    {
        messages[index].push_back({"", false});

        size_t beg = tmp.find("BEG", begOffset);
        if (beg != string::npos)
        {
            size_t end = tmp.find("END", beg + 3);
            if (end != string::npos)
            {
                messages[index][curMsg].first = tmp.substr(beg + 3, end - beg - 3);		
                messages[index][curMsg].second = true;

                constructFineMessage(buffer, size, index, curMsg + 1, end + 3);
                return;
            }
            else
            {
                messages[index][curMsg].first = tmp.substr(beg + 3);
                messages[index][curMsg].second = false;

                return;
            }
        }	
    }
    else
    {
        size_t end = tmp.find("END", begOffset);
        if (end != string::npos)
        {
            messages[index][curMsg].first += tmp.substr(begOffset, end);
            messages[index][curMsg].second = true;

            constructFineMessage(buffer, size, index, curMsg + 1, end + 3);
            return;
        }
        else
        {
            messages[index][curMsg].first += tmp;
            messages[index][curMsg].second = false;

            return;
        }
    }

    return;
    /* END */
}

void Node::checkConnections(int size)
{
    for (int i = 0; i < max_clients; i++)
    {
        int inputsd = client_sockets[i];

        if (FD_ISSET(inputsd, &readfds))
        {
            char* buffer = new char[size + 1];
            int bytes_read = 1;

            if (!messages[i].empty())
            {
                messages[i].pop_front();
            }

            while (messages[i].empty() || (!messages[i].empty() && !messages[i][0].second))
            {	
                memset(buffer, 0, size + 1);

                bytes_read = recv(inputsd, buffer, size, MSG_DONTWAIT);
               
                /* disconnected */
                if (bytes_read == 0)
                {
                    break;
                }
                else if (bytes_read < 0)
                {
                    break;
                }

                constructFineMessage(buffer, bytes_read, i, 0, 0);
            }
            
            if (!messages[i].empty() && messages[i][0].second)
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

            delete[] buffer;

            /* client has disconnected */
            if (bytes_read == 0)
            {
                old_client_sockets[i] = client_sockets[i];
                client_sockets[i] = 0;
                new_client_sockets[i] = 0;

                messages[i].clear();

                close(inputsd);
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

    unique_lock < mutex > lck(mtx);
    ready = false;

    checkNewConnections();
    checkConnections(size);

    ready = true;
    cv.notify_all();
}

void Node::sendMSG(int to, string msg, bool force)
{
    if (msg.empty() || msg == "" || to <= 0)
    {
        return;
    }

    /* check the same message */
    for (size_t i = 0; i < client_sockets.size(); i++)
    {
        if (client_sockets[i] == to || new_client_sockets[i] == to)
        {
            if (lastMsgs[i] == msg)
            {
                return;
            }

            break;
        }
    }

    if (send(to, msg.data(), msg.size(), MSG_NOSIGNAL) < 0)
    {
        throw(runtime_error("ERROR::Node::sendMSG() send"));
    }

    /* save the message */
    for (size_t i = 0; i < client_sockets.size(); i++)
    {
        if (client_sockets[i] == to || new_client_sockets[i] == to)
        {
            lastMsgs[i] = msg;
            break;
        }
    }
}

vector < int > Node::getClientSockets() const
{
    return client_sockets;
}

vector < int > Node::getNewClientSockets() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    return new_client_sockets;
}

vector < int > Node::getOldClientSockets() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    return old_client_sockets;
}

bool Node::isNewClients() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    for (size_t i = 0; i < new_client_sockets.size(); i++)
    {
        if (new_client_sockets[i] > 0)
        {
            return true;
        }
    }

    return false;
}

bool Node::isOldClients() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    for (size_t i = 0; i < old_client_sockets.size(); i++)
    {
        if (old_client_sockets[i] > 0)
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
        if (!messages[i].empty() && messages[i][0].second)
        {
            res.push_back(messages[i][0].first);
        }
        else
        {
            res.push_back("");
        }
    }

    return move(res);
}

void Node::newToClient(int index)
{
    client_sockets[index] = new_client_sockets[index];
    new_client_sockets[index] = 0;
}

void Node::oldToNothing(int index)
{
    old_client_sockets[index] = 0; 
}

Node::~Node() {}
