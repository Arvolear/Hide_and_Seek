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
        WeaponDataCollector* weaponDataCollector;
        WeaponPickerCollector* weaponPickerCollector;
        WeaponPickerUpdater* weaponPickerUpdater;
        WeaponDropperCollector* weaponDropperCollector;
        WeaponDropperUpdater* weaponDropperUpdater;
        PlayerConnectionCollector* playerConnectionCollector;
        PlayerDisconnectionCollector* playerDisconnectionCollector;

        Level* level;
        World* world;

    public:
        Multiplayer(Level* level, World* world);

        void broadcast();
        void update();

        void finish();

        ~Multiplayer();
};
