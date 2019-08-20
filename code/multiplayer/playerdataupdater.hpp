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
        int playerID;

        mat4 localTransform;
        mat4 model;

        Animation* animation;

    public:
        PlayerDataUpdater();

        void collect(string info);

        void updateData(Player* player);

        int getPlayerID() const;
        
        ~PlayerDataUpdater();
};
