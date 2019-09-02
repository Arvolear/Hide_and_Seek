#pragma once

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

struct Picked
{
    btRigidBody* body = 0;
    btTypedConstraint* constraint = 0;
    btScalar distance;
};

class ConstraintHandler
{
    private:
        btDynamicsWorld* world;
        RayTracer* rayTracer;

        Camera* camera;
        mat4 projection;
        
        Picked pickedObject;

    public:
        ConstraintHandler(btDynamicsWorld* world);
        ConstraintHandler(btDynamicsWorld* world, Camera* camera, mat4 &projection);

        void setCamera(Camera* camera);
        void setProjection(mat4 &projection);
        
        void create(float posx, float posy, bool pickStatic = false);
        void update(float posx, float posy);
        void remove();

        RayTracer* getRayTracer() const;

        ~ConstraintHandler();
};
