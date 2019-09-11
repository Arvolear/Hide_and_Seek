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

    public:
        PlayerDisconnectionCollector();

        void collect(vector < int > old_clients);

        string getData() const;

        void clear();
        
        ~PlayerDisconnectionCollector();
};
