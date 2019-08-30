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

        void setBTTransform(btTransform* transform);

        btScalar* getGLTransform() const;
        btTransform* getBTTransform() const;
        
        void update();

        ~OpenGLMotionState();
};
