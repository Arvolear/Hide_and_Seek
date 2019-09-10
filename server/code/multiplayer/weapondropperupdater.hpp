#pragma once

#include <stdexcept>
#include <vector>
#include <map>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class WeaponDropperUpdater
{
    private:
        int playerID;

        btScalar* model;

    public:
        WeaponDropperUpdater();

        void collect(string info);

        void updateData(Player* player);

        int getPlayerID() const;

        void clear();

        ~WeaponDropperUpdater();
};
