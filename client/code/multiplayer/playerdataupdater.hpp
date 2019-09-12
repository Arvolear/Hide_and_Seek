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

        mat4 model;
        vec3 moveDirection;

    public:
        PlayerDataUpdater();

        void collect(string info);

        void updateData(Player* player, bool interpolation = false);

        int getPlayerID() const;

        void clear();
        
        ~PlayerDataUpdater();
};
