#pragma once

//native
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

class Spawner
{
    private:
        vector < btVector3 > positions;

        vector < int > prev;

    public:
        Spawner(int clients);

        void addPosition(btVector3 position);
        
        btTransform getTransform(int client);

        void clearPrev(int client);

        ~Spawner();
};
