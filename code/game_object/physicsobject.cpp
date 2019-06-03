#include "openglmotionstate.hpp"
#include "physicsobject.hpp"

PhysicsObject::PhysicsObject(btCollisionShape *shape, float mass, const btVector3 &initialPosition, const btQuaternion &initialRotation)
{
    this->shape = shape;

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(initialPosition);
    transform.setRotation(initialRotation);

    motionState = new OpenGLMotionState(transform);

    btVector3 localInertia(0, 0, 0);

    if (mass)
    {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, shape, localInertia);

    body = new btRigidBody(cInfo);
}

PhysicsObject::PhysicsObject(vector < PhysicsObjectCompound* > &compoundInfo, float mass, const btVector3 &initialPosition, const btQuaternion &initialRotation, bool clearVector)
{
    compound = new btCompoundShape();

    for (size_t i = 0; i < compoundInfo.size(); i++)
    {
        shapes.push_back(compoundInfo[i]->bodyShape);

        btTransform localTransform;

        localTransform.setIdentity();
        localTransform.setOrigin(compoundInfo[i]->position);
        localTransform.setRotation(compoundInfo[i]->rotation);
        compound->addChildShape(localTransform, compoundInfo[i]->bodyShape);
    }

    shape = compound;
    
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(initialPosition);
    transform.setRotation(initialRotation);

    motionState = new OpenGLMotionState(transform);

    btVector3 localInertia(0, 0, 0);

    if (mass)
    {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, compound, localInertia);

    body = new btRigidBody(cInfo);

    if (clearVector)
    {
        for (size_t i = 0; i < compoundInfo.size(); i++)
        {
            delete compoundInfo[i];
        }

        compoundInfo.clear();
    }
}

btRigidBody* PhysicsObject::getRigidBody() const
{
    return body;
}

btMotionState* PhysicsObject::getMotionState() const
{
    return motionState;
}

void PhysicsObject::getTransform(btScalar* transform) const
{
    motionState->getTransform(transform);
}

PhysicsObject::~PhysicsObject()
{
    delete shape;
    //delete compound;

    for (size_t i = 0; i < shapes.size(); i++)
    {
        delete shapes[i];
    }

    delete body;

    delete motionState;
}
