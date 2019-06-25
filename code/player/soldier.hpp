#pragma once

//native
#include <vector>
#include <stdexcept>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

class Soldier : public Player
{
    private:

    public:
        Soldier();
        
        ~Soldier();
};
