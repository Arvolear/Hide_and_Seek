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

        Camera* camera;
        mat4 projection;

    public:
        RayTracer(btDynamicsWorld* world);
        RayTracer(btDynamicsWorld* world, Camera* camera, mat4 &projection);

        void setCamera(Camera* camera);
        void setProjection(mat4 &projection);

        btVector3 getPickingRay(double posx, double posy) const;
        RayResult* rayCast(btVector3 &rayFrom, btVector3 &rayTo, bool checkStatic = true) const;

        ~RayTracer();
};
