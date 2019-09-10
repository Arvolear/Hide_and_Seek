#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PhysicsObjectDataUpdater
{
    private:
        string name;
        int senderID;

        btScalar* model;
    
    public:
        PhysicsObjectDataUpdater();

        void collect(string info);

        void updateData(PhysicsObject* physicsObject);

        string getName() const;
        int getSenderID() const;

        void clear();

        ~PhysicsObjectDataUpdater();
};
