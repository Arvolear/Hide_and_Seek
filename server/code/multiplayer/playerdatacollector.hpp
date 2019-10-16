#pragma once

#include <stdexcept>
#include <vector>
#include <string>
#include <map>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PlayerDataCollector
{
    private:
        vector < int > playerIDs;
        mutable map < int, btScalar* > models;
        mutable map < int, btVector3 > moveDirections;
        mutable map < int, vector < string > > pickedWeapons;

    public:
        PlayerDataCollector();

        void collect(Player* player);
        void collect(vector < Player* > players);

        string getData(bool weapons = false, bool raw = false) const;
        string getMergedData(string fileName, bool weapons = false, bool raw = false) const;

        void clear();
        
        ~PlayerDataCollector();
};
