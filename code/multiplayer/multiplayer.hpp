#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <mutex>

using namespace std;

class Multiplayer
{
    private:
        Client* client;
        PlayerDataCollector* playerDataCollector;
        PlayerDataUpdater* playerDataUpdater;

        Window* window;
        Level* level;

    public:
        Multiplayer(Window* window, Level* level);

        void connect();
        
        void broadcast();
        void update();

        ~Multiplayer();
};
