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
        vector < int > playerIDs;
        
        vector < btScalar* > models;
        vector < btVector3 > moveDirections;

        vector < vector < string > > pickedWeapons;

    public:
        PlayerDataCollector();

        void collect(Player* player);
        void collect(vector < Player* > players);

        string getData(bool weapons = false, bool raw = false) const;

        void clear();
        
        ~PlayerDataCollector();
};
