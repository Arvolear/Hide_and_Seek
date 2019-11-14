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

        mutable vector < string > last;

    public:
        WeaponDataCollector(int clients);

        void collect(map < string, PhysicsObject* > physicsObjects);

        string getMergedData(string fileName, int client, bool raw = false) const;

        void clear(); 
        void clearLast(int client);
        void clearAllLast();

        ~WeaponDataCollector();
};
