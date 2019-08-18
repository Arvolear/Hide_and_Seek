#include "openglmotionstate.hpp"

OpenGLMotionState::OpenGLMotionState(btTransform* transform) : btDefaultMotionState(*transform)
{
    this->transform = transform;
}

void OpenGLMotionState::setGLTransform(btScalar* GLtransform)
{
    transform->setFromOpenGLMatrix(GLtransform);
    update();
}

btScalar* OpenGLMotionState::getGLTransform() const
{
    btScalar* GLtransform = new btScalar[16];

    getWorldTransform(*transform);
    transform->getOpenGLMatrix(GLtransform);

    return GLtransform;
}
        
btTransform* OpenGLMotionState::getBTTransform() const
{
    return transform;
}

void OpenGLMotionState::update()
{
    setWorldTransform(*transform);
}

OpenGLMotionState::~OpenGLMotionState()
{
    delete transform;
}
