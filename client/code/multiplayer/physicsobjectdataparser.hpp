//client

#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class PhysicsObjectDataParser
{
    private:
        vector < string > names;
        map < string, btCollisionShape* > collShapes; 
        map < string, CompoundShape* > compShapes;
        map < string, float > masses;
        map < string, mat4 > models;
        map < string, vec3 > aFactors; 

    public:
        PhysicsObjectDataParser();

        void parse(XMLElement* objElem);
        
        void updatePhysicsObject(GameObject* gameObject, bool interpolation = false, unsigned int timeStamp = 0);

        vector < string > getNames() const;

        void clear();

        ~PhysicsObjectDataParser();
};
