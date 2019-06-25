#pragma once

#include <string>
#include <vector>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Weapon : public GameObject
{
    protected:

    public:
        Weapon();

        void setRelativePosition();
        void reload();
        void drop();
        void pick();
        void fire();

        ~Weapon();
};
