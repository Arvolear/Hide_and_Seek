#pragma once

#include <stdexcept>
#include <vector>
#include <map>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class WeaponFireUpdater
{
    private:
        World* world;
        RayTracer* rayTracer;

        int playerID;
       
        map < string, vector < pair < btVector3, btVector3 > > > fireInfo;

    public:
        WeaponFireUpdater(World* world);

        void collect(string info);

        void updateData(Player* player, map < string, PhysicsObject* > physicsObjects);

        int getPlayerID() const;

        void clear();

        ~WeaponFireUpdater();
};
