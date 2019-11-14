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
        mutable map < string, btScalar* > pos;

        mutable vector < string > last;

    public:
        PhysicsObjectDataCollector(int clients);

        void collect(PhysicsObject* physicsObject);
        void collect(map < string, PhysicsObject* > physicsObjects);

        string getData(int client, bool raw = false) const;
        string getMergedData(string fileName, int client, bool raw = false) const;

        void clear();
        void clearLast(int client);
        void clearAllLast();

        ~PhysicsObjectDataCollector();
};
