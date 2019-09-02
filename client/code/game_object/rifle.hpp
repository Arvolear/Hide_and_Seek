#pragma once

#include <string>
#include <vector>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Rifle : public Weapon
{
    public:
        Rifle(Window* window, string name);

        bool fire() override;

        ~Rifle();
};
