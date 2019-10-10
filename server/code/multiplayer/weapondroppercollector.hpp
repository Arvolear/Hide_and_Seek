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

    public:
        WeaponDropperCollector();

        void collect(Player* player);

        string getData() const;

        void clear();

        ~WeaponDropperCollector();
};
