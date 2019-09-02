//client

#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class GameObjectDataUpdater
{
    private:
        string name;

        mat4 model;
    
    public:
        GameObjectDataUpdater();

        void collect(string info);

        void updateData(GameObject* gameObject);

        string getName() const;

        ~GameObjectDataUpdater();
};
