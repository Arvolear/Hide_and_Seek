#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class GameObjectDataCollector
{
    private:
        string name;
        int senderID;

        mat4 model;        

    public:
        GameObjectDataCollector();

        void setSenderID(int senderID);

        void collect(GameObject* gameObject);

        string getData() const;

        void clear();

        ~GameObjectDataCollector();
};
