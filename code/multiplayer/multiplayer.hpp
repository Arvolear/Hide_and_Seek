#pragma once

#include <iostream>
#include <string>
#include <thread>

using namespace std;

class Multiplayer
{
    private:
        Client* client;
        PlayerDataCollector* playerDataCollector;
        PlayerDataUpdater* playerDataUpdater;

        Level* level;

    public:
        Multiplayer();

        void connect();

        void update();

        ~Multiplayer();
};
