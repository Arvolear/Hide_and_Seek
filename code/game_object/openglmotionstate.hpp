#pragma once

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

class OpenGLMotionState : public btDefaultMotionState
{
    private:
        btTransform* transform;

    public:
        OpenGLMotionState(btTransform* transform);

        void setGLTransform(btScalar* GLtransform);

        btScalar* getGLTransform() const;
        btTransform* getBTTransform() const;
        
        void update();

        ~OpenGLMotionState();
};
