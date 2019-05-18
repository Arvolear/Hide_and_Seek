#pragma once

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

class OpenGLMotionState : public btDefaultMotionState
{
    public:
        OpenGLMotionState(btTransform &transform);

        void getTransform(btScalar *transform) const;

        ~OpenGLMotionState();
};
