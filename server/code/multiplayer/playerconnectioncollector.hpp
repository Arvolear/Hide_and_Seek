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

    public:
        PlayerConnectionCollector();

        void collect(vector < int > clients, int playerID);

        string getData() const;

        void clear();
        
        ~PlayerConnectionCollector();
};
