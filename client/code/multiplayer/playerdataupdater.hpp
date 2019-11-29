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
 
        map < int, float > speeds;
        map < int, vec3 > moveDirections;
        
        PhysicsObjectDataParser* objParser;

        map < int, vector < string > > pickedWeapons;

        map < int, int > healths;

        unsigned int timeStamp;

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
