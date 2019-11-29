//client

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
        GameObjectDataCollector* gameObjectDataCollector;
        GameObjectDataUpdater* gameObjectDataUpdater;
        WeaponDataUpdater* weaponDataUpdater;
        WeaponPickerCollector* weaponPickerCollector;
        WeaponPickerUpdater* weaponPickerUpdater;
        WeaponDropperCollector* weaponDropperCollector;
        WeaponDropperUpdater* weaponDropperUpdater;
        WeaponFireCollector* weaponFireCollector;
        PlayerConnectionUpdater* playerConnectionUpdater;
        PlayerDisconnectionUpdater* playerDisconnectionUpdater;

        Window* window;
        Level* level;
        World* world;

        int playerID;

    public:
        Multiplayer(Window* window, Level* level, World* world);

        void connect();
        
        void broadcast();
        void update();

        ~Multiplayer();
};
