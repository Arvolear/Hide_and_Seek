#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PlayerDisconnectionCollector
{
    private:
        vector < int > old_clients;

        mutable vector < string > last;

    public:
        PlayerDisconnectionCollector(int clients);

        void collect(vector < int > old_clients);

        string getData(int client) const;

        void clear();
        void clearLast(int client);
        void clearAllLast();
        
        ~PlayerDisconnectionCollector();
};
