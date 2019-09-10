#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class WeaponDropperCollector
{
    private:
        int playerID;

        bool dropTo;
        mat4 model;

    public:
        WeaponDropperCollector();

        void setPlayerID(int playerID);

        void collect(Player* player);

        string getData() const;

        void clear();

        ~WeaponDropperCollector();
};
