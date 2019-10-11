#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PlayerDataUpdater
{
    private:
        vector < int > playerIDs;

        vector < mat4 > models;
        vector < vec3 > moveDirections;
        vector < vector < string > > pickedWeapons;

    public:
        PlayerDataUpdater();

        void collect(string info);

        void updateData(Player* player, bool interpolation = false, map < string, GameObject* > gameObjects = {});
        void updateData(vector < Player* > players, bool interpolation = false, map < string, GameObject* > gameObjects = {});

        int getPlayerID(int index = 0) const;
        vector < int > getPlayerIDs() const;
        
        void clear();
        
        ~PlayerDataUpdater();
};
