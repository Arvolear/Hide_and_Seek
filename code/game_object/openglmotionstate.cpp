#include "openglmotionstate.hpp"

OpenGLMotionState::OpenGLMotionState(btTransform &transform) : btDefaultMotionState(transform){}

void OpenGLMotionState::getTransform(btScalar *transform) const
{
    btTransform trans;

    getWorldTransform(trans);
    trans.getOpenGLMatrix(transform);
}

OpenGLMotionState::~OpenGLMotionState(){}
