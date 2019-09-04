//server

#pragma once

#include <stdexcept>
#include <vector>
#include <map>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PhysicsObjectDataCollector
{
    private:
        vector < string > names;
        vector < btScalar* > models;        

    public:
        PhysicsObjectDataCollector();

        void collect(PhysicsObject* physicsObject);
        void collect(map < string, PhysicsObject* > physicsObjects);

        string getData(bool raw = false) const;

        void clear(); 

        ~PhysicsObjectDataCollector();
};
