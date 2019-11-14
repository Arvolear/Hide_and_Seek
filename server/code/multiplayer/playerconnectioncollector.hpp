#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PlayerConnectionCollector
{
    private:
        vector < int > clients;

        mutable vector < string > last;

    public:
        PlayerConnectionCollector(int clients);

        void collect(vector < int > clients, int playerID);

        string getData(int client) const;

        void clear();
        void clearLast(int client);
        void clearAllLast();
        
        ~PlayerConnectionCollector();
};
