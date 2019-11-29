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

        RayResult* rayCast(btRigidBody* me, btVector3 rayFrom, btVector3 rayTo, bool missStatic = true, bool missKinematic = true) const;

        ~RayTracer();
};
