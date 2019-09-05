#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class WeaponPickerCollector
{
    private:
        int playerID;

        vec3 pickFrom;
        vec3 pickTo;

    public:
        WeaponPickerCollector();

        void setPlayerID(int playerID);

        void collect(Player* player);

        string getData() const;

        void clear();

        ~WeaponPickerCollector();
};
