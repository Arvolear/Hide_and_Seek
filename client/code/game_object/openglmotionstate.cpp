#include "openglmotionstate.hpp"

OpenGLMotionState::OpenGLMotionState(btTransform* transform) : btDefaultMotionState(*transform)
{
    this->transform = transform;
}

void OpenGLMotionState::setGLTransform(btScalar* transform)
{
    this->transform->setFromOpenGLMatrix(transform);
    update();
}

void OpenGLMotionState::setBTTransform(btTransform* transform)
{
    delete this->transform;

    this->transform = transform;
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
