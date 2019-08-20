#include "server.hpp"

Server::Server(int max_clients, int max_queue, int port)
{
    this->max_clients = max_clients;
    client_sockets.resize(max_clients);

    master_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (master_sock < 0)
    {
        throw(runtime_error("server socket"));
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(master_sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        throw(runtime_error("server bind"));
    }

    if (listen(master_sock, max_queue) < 0)
    {
        throw(runtime_error("server listen"));
    }
}

void Server::checkNewConnections()
{
    if (FD_ISSET(master_sock, &readfds))
    {
        int client_socket = accept(master_sock, NULL, NULL);

        if (client_socket < 0)
        {
            throw(runtime_error("server accept"));
        }

        /* welcome */
        int emptyInd = -1;
        bool newSock = true;

        for (int i = 0; i < max_clients; i++)
        {
            /* empty socket cell */
            if (!client_sockets[i] && emptyInd == -1)
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
            string message = "header\n<connected>" + to_string(emptyInd) + "</connected>\nfooter\n";
            send(client_socket, message.data(), message.size(), 0);

            client_sockets[emptyInd] = client_socket;
        }
    }
}

bool Server::constructFineMessage(char* buffer, int size)
{
	/* header */

	if (!buffer || size <= 0)
	{
		return false;
	}

	string tmp(buffer, size);

	if (fineMessage.empty())
	{
		size_t head = tmp.find("header");
		if (head != string::npos)
		{
			size_t foot = tmp.find("footer");
			if (foot != string::npos)
			{
                if (foot > head)
                {
    				fineMessage = tmp.substr(head, foot - head + 6);		
	    			return true;
                }
            }
            else
            {
                fineMessage = tmp.substr(head);
                return false;
            }
        }	
    }
    else
    {
        size_t foot = tmp.find("footer");
        if (foot != string::npos)
        {
            fineMessage += tmp.substr(0, foot + 6);		
            return true;
        }
    }

    return true;
    /* footer */
}

void Server::checkOldConnections(int size)
{
    for (int i = 0; i < max_clients; i++)
    {
        int inputsd = client_sockets[i];

        if (FD_ISSET(inputsd, &readfds))
        {
            char* buffer = new char[size + 1];

            int bytes_read = 0;

            while (true)
            {	
                memset(buffer, 0, size);

                bytes_read = recv(inputsd, buffer, size, 0);

                if (bytes_read <= 0 || constructFineMessage(buffer, bytes_read))
                {
                    break;
                }
            }

            //cout << fineMessage << endl << endl;

            delete[] buffer;

            /* client has disconnected */
            if (bytes_read <= 0)
            {
                string message = "header\n<disconnected>" + to_string(i) + "</disconnected>\nfooter\n";

                for (int j = 0; j < max_clients; j++)
                {
                    int outputsd = client_sockets[j];

                    if (outputsd != 0 && inputsd != outputsd)
                    {
                        send(outputsd, message.data(), message.size(), 0);
                    }
                }

                close(inputsd);
                client_sockets[i] = 0;
            }
            /* else receive and broadcast the message */
            else
            {
                for (int j = 0; j < max_clients; j++)
                {
                    int outputsd = client_sockets[j];

                    if (outputsd != 0 && inputsd != outputsd)
                    {
                        send(outputsd, fineMessage.data(), fineMessage.size(), 0);
                    }
                }
            }

            fineMessage.clear();
        }
    }
}

void Server::routeMSG(int size)
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

    int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

    if (activity < 0 && errno != EINTR)
    {
        throw(runtime_error("server select"));
    }

    checkNewConnections();
    checkOldConnections(size);
}
