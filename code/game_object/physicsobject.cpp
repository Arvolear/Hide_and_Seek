#include "openglmotionstate.hpp"
#include "physicsobject.hpp"
        
CompoundShape::CompoundShape()
{
    shape = new btCompoundShape();
}
        
void CompoundShape::setShape(btCompoundShape* shape)
{
    this->shape = shape;
}

void CompoundShape::add(btCollisionShape* childShape, btVector3 position, btQuaternion rotation)
{
    btTransform localTransform;

    localTransform.setIdentity();
    localTransform.setOrigin(position);
    localTransform.setRotation(rotation);
    shape->addChildShape(localTransform, childShape);

    childShapes.push_back(childShape);
}
        
btCollisionShape* CompoundShape::getShape() const
{
    return shape;
}
        
CompoundShape::~CompoundShape()
{
    //delete shape;

    for (size_t i = 0; i < childShapes.size(); i++)
    {
        delete childShapes[i];
    }
}

/********* PHYSICS OBJECT **********/

void PhysicsObject::updateBody(btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation)
{
    if (body)
    {
        world->removeRigidBody(body);
        delete body;
    }

    this->phShape = shape;
    this->mass = mass;

    btVector3 localInertia(0, 0, 0);

    if (mass)
    {
        this->phShape->calculateLocalInertia(mass, localInertia);
    }

    btTransform* transform = motionState->getBTTransform();
    transform->setOrigin(position);
    transform->setRotation(rotation);
    motionState->update();

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, phShape, localInertia);

    body = new btRigidBody(cInfo);

    world->addRigidBody(body);  
}        

PhysicsObject::PhysicsObject(btDynamicsWorld* world)
{
    this->world = world;
    this->mass = 0;
    this->phShape = nullptr;
    this->comShape = nullptr;
    this->body = nullptr;

    btTransform* transform = new btTransform();
    transform->setIdentity();

    motionState = new OpenGLMotionState(transform);
}

PhysicsObject::PhysicsObject(btDynamicsWorld* world, btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation)
{
    this->world = world;
    this->mass = 0;
    this->body = nullptr;
    this->comShape = nullptr;

    btTransform* transform = new btTransform();
    transform->setIdentity();

    motionState = new OpenGLMotionState(transform);
    
    updateBody(shape, mass, position, rotation);
}

PhysicsObject::PhysicsObject(btDynamicsWorld* world, CompoundShape* shape, float mass, btVector3 position, btQuaternion rotation)
{
    this->world = world;
    this->mass = 0;
    this->body = nullptr;
    this->comShape = nullptr;

    btTransform* transform = new btTransform();
    transform->setIdentity();

    motionState = new OpenGLMotionState(transform);
    
    this->comShape = shape;
    
    updateBody(shape->getShape(), mass, position, rotation);
}

void PhysicsObject::setShape(btCollisionShape* shape)
{
    delete this->phShape;
    this->phShape = nullptr;
    delete this->comShape;
    this->comShape = nullptr;

    updateBody(shape, mass, motionState->getBTTransform()->getOrigin(), motionState->getBTTransform()->getRotation());
}

void PhysicsObject::setShape(CompoundShape* shape)
{
    delete this->phShape;
    this->phShape = nullptr;
    delete this->comShape;

    this->comShape = shape;

    updateBody(shape->getShape(), mass, motionState->getBTTransform()->getOrigin(), motionState->getBTTransform()->getRotation());
}

void PhysicsObject::setMass(float mass)
{
    updateBody(phShape, mass, motionState->getBTTransform()->getOrigin(), motionState->getBTTransform()->getRotation());
}

void PhysicsObject::setPosition(btVector3 position)
{
    updateBody(phShape, mass, position, motionState->getBTTransform()->getRotation());
}

void PhysicsObject::setRotation(btQuaternion rotation)
{
    updateBody(phShape, mass, motionState->getBTTransform()->getOrigin(), rotation);
}

btRigidBody* PhysicsObject::getRigidBody() const
{
    return body;
}

btCollisionShape* PhysicsObject::getShape() const
{
    return phShape;
}

btScalar* PhysicsObject::getTransform() const
{
    return motionState->getGLTransform();
}

PhysicsObject::~PhysicsObject()
{
    if (body)
    {
        world->removeRigidBody(body);
    }

    delete phShape;
    delete comShape;
    delete body;
    delete motionState;
}
