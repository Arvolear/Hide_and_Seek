#pragma once

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

//openGL
#include <glm/glm.hpp>

using namespace glm;

struct RayResult
{
    btRigidBody* body;
    btVector3 hitPoint;
};

class RayTracer
{
    private:
        btDynamicsWorld* world;

    public:
        RayTracer(btDynamicsWorld* world);

        btVector3 getPickingRay(double posx, double posy) const;
        RayResult* rayCast(btVector3 &rayFrom, btVector3 &rayTo, bool missStatic = true) const;

        ~RayTracer();
};
