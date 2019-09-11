#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PlayerDisconnectionUpdater
{
    private:
        vector < int > playerIDs;

    public:
        PlayerDisconnectionUpdater();

        void collect(string info);

        void updateData(Player* player);

        vector < int > getPlayerIDs() const;

        void clear();
        
        ~PlayerDisconnectionUpdater();
};
