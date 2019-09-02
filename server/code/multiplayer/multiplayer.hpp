//server

#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <tinyxml2/tinyxml2.h>

using namespace tinyxml2;
using namespace std;

class Multiplayer
{
    private:
        Node* node;
        PlayerDataCollector* playerDataCollector;
        PlayerDataUpdater* playerDataUpdater;
        PhysicsObjectDataCollector* physicsObjectDataCollector;
        PhysicsObjectDataUpdater* physicsObjectDataUpdater;

        Level* level;

    public:
        Multiplayer(Level* level);

        void broadcast();
        void update();

        void finish();

        ~Multiplayer();
};
