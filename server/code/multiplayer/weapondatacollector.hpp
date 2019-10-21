#pragma once

#include <stdexcept>
#include <vector>
#include <map>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class WeaponDataCollector
{
    private:
        mutable map < string, btScalar* > pos;
        mutable map < string, int > storages;
        mutable map < string, int > sizes;
        mutable map < string, int > magazines;
        mutable map < string, float > speeds;
        mutable map < string, float > powers;

    public:
        WeaponDataCollector();

        void collect(map < string, PhysicsObject* > physicsObjects);

        string getMergedData(string fileName, bool raw = false) const;

        void clear(); 

        ~WeaponDataCollector();
};
