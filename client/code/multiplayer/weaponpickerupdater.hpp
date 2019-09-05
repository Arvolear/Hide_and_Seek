#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class WeaponPickerUpdater
{
    private:
        int playerID;

        vector < string > names;
        
    public:
        WeaponPickerUpdater();

        void collect(string info);

        void updateData(Player* player, GameObject* gameObject);
        
        int getPlayerID() const;
        vector < string > getNames() const;

        void clear();

        ~WeaponPickerUpdater();
};
