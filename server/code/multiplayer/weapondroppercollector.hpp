#pragma once

#include <stdexcept>
#include <vector>
#include <deque>
#include <map>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class WeaponDropperCollector
{
    private:
        int playerID;

        vector < string > names;

        mutable vector < string > last;

    public:
        WeaponDropperCollector(int clients);

        void collect(Player* player);

        string getData(int client) const;

        void clear();
        void clearLast(int client);
        void clearAllLast();

        ~WeaponDropperCollector();
};
