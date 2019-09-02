//server

#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PhysicsObjectDataCollector
{
    private:
        string name;
        btScalar* model;        

    public:
        PhysicsObjectDataCollector();

        void collect(PhysicsObject* physicsObject);

        string getData() const;

        void clear(); 

        ~PhysicsObjectDataCollector();
};
