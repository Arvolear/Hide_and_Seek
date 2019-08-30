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

        btVector3 Forward;
        btVector3 Up;
        string move;

    public:
        PlayerDataUpdater();

        void collect(string info);

        void updateData(Player* player);

        int getPlayerID() const;

        void clear();
        
        ~PlayerDataUpdater();
};
