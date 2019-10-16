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
        mutable map < string, btScalar* > info;

    public:
        PhysicsObjectDataCollector();

        void collect(PhysicsObject* physicsObject);
        void collect(map < string, PhysicsObject* > physicsObjects);

        string getData(bool raw = false) const;
        string getMergedData(string fileName, bool raw = false) const;

        void clear(); 

        ~PhysicsObjectDataCollector();
};
