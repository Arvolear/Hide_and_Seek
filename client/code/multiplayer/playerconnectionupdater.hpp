#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PlayerConnectionUpdater
{
    private:
        vector < int > playerIDs;

    public:
        PlayerConnectionUpdater();

        void collect(string info);

        void updateData(Player* player);

        vector < int > getPlayerIDs() const;

        void clear();
        
        ~PlayerConnectionUpdater();
};
