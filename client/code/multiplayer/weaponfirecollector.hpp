#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class WeaponFireCollector
{
    private:
        int playerID;

        map < string, vector < pair < vec3, vec3 > > > fireInfo;
        mutable map < string, bool > reloadInfo;

    public:
        WeaponFireCollector();

        void setPlayerID(int playerID);

        void collect(Player* player);

        string getData() const;

        void clear();

        ~WeaponFireCollector();
};
