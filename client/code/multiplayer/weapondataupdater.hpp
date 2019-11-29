#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class WeaponDataUpdater
{
    private:
        PhysicsObjectDataParser* objParser;
        vector < string > names;

        map < string, int > storages;
        map < string, int > sizes;
        map < string, int > magazines;
        map < string, float > speeds;
        map < string, int > powers;

    public:
        WeaponDataUpdater();

        void collect(string info);

        void updateData(map < string, GameObject* > gameObjects, bool interpolation = false);

        vector < string > getNames() const;

        void clear();

        ~WeaponDataUpdater();
};
