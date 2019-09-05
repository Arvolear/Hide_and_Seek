#pragma once

#include <stdexcept>
#include <vector>
#include <map>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class WeaponPickerUpdater
{
    private:
        World* world;
        RayTracer* rayTracer;

        int playerID;
        btVector3 pickFrom;
        btVector3 pickTo;
        
        Weapon* weapon;

    public:
        WeaponPickerUpdater(World* world);

        void collect(string info);

        void updateData(Player* player);

        int getPlayerID() const;

        void clear();

        ~WeaponPickerUpdater();
};
