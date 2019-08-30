#pragma once

#include <iostream>
#include <string>
#include <tinyxml2/tinyxml2.h>

using namespace tinyxml2;
using namespace std;

class Multiplayer
{
    private:
        Client* client;
        PlayerDataCollector* playerDataCollector;
        PlayerDataUpdater* playerDataUpdater;

        Window* window;
        Level* level;

        int playerID;

    public:
        Multiplayer(Window* window, Level* level);

        void connect();
        
        void broadcast();
        void update();

        ~Multiplayer();
};
