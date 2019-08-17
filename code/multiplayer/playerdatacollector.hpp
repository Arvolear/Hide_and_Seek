#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PlayerDataCollector
{
    private:
        int playerID;

        mat4 localTransform;
        mat4 model;

        bool meshWithBones;
        vector < mat4 > bones;

    public:
        PlayerDataCollector();

        void setPlayerID(int playerID);
        
        void collect(Player* player);

        string getData() const;
        
        ~PlayerDataCollector();
};
