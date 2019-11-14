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
        PhysicsObjectDataParser* objParser;
        vector < string > names;

        unsigned int timeStamp;

    public:
        GameObjectDataUpdater();

        void collect(string info);

        void updateData(GameObject* gameObject, bool interpolation = false);
        void updateData(map < string, GameObject* > gameObjects, bool interpolation = false);

        string getName(int index = 0) const;
        vector < string > getNames() const;

        void clear();

        ~GameObjectDataUpdater();
};
