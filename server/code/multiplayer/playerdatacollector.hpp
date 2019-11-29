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

        vector < string > names;
        mutable map < int, btScalar* > models;
        mutable map < int, btVector3 > moveDirections;
        mutable map < int, vector < string > > pickedWeapons;
        mutable map < int, int > healths;

        mutable vector < string > last;

    public:
        PlayerDataCollector(int clients);

        void collect(Player* player);
        void collect(vector < Player* > players);

        string getData(int client, bool position = true, bool health = false, bool weapons = false, bool raw = false) const;
        string getMergedData(string fileName, int client, bool posotion = true, bool health = false, bool weapons = false, bool raw = false) const;

        void clear();
        void clearLast(int client);
        void clearAllLast();
        
        ~PlayerDataCollector();
};
