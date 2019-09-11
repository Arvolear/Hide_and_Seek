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
        
btCompoundShape* CompoundShape::getShape() const
{
    return shape;
}
        
btCollisionShape* CompoundShape::getChildShape(unsigned int index) const
{
    if (index < 0 || index >= childShapes.size())
    {
        throw range_error("ERROR compound shape has no " + to_string(index) + " child");
    }

    return childShapes[index];
}
        
btTransform CompoundShape::getChildTransform(unsigned int index) const
{
    if (index < 0 || index >= childShapes.size())
    {
        throw range_error("ERROR compound shape has no " + to_string(index) + " child");
    }

    btTransform transform;
    transform = shape->getChildTransform(index);

    return transform;
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
    if (!shape)
    {
        return;
    }

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
        this->stat = false;
    }
    else
    {
        this->stat = true;
    }

    btTransform* transform = motionState->getBTTransform();
    transform->setOrigin(position);
    transform->setRotation(rotation);
    motionState->update();

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, phShape, localInertia);

    body = new btRigidBody(cInfo);
    body->setUserPointer(this);

    world->addRigidBody(body);

    if (!collidable)
    {
        body->setCollisionFlags(btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);
    }
}        

PhysicsObject::PhysicsObject(btDynamicsWorld* world)
{
    this->world = world;
    this->mass = 0;
    this->phShape = nullptr;
    this->comShape = nullptr;
    this->body = nullptr;
    this->collidable = true;
    this->stat = true;
    this->userPointer = nullptr;

    btTransform* transform = new btTransform();
    transform->setIdentity();

    motionState = new OpenGLMotionState(transform);
}

PhysicsObject::PhysicsObject(btDynamicsWorld* world, btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation)
{
    this->world = world;
    this->mass = mass;
    this->body = nullptr;
    this->comShape = nullptr;
    this->collidable = true;
    this->stat = mass ? false : true;
    this->userPointer = nullptr;

    btTransform* transform = new btTransform();
    transform->setIdentity();

    motionState = new OpenGLMotionState(transform);
    
    updateBody(shape, mass, position, rotation);
}

PhysicsObject::PhysicsObject(btDynamicsWorld* world, CompoundShape* shape, float mass, btVector3 position, btQuaternion rotation)
{
    this->world = world;
    this->mass = mass;
    this->body = nullptr;
    this->comShape = nullptr;
    this->collidable = true;
    this->stat = mass ? false : true;
    this->userPointer = nullptr;

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

void PhysicsObject::setMass(float mass, bool add)
{
    if (add)
    {
        mass += this->mass;
    }
        
    updateBody(phShape, mass, motionState->getBTTransform()->getOrigin(), motionState->getBTTransform()->getRotation());
}

void PhysicsObject::setPosition(btVector3 position, bool add)
{
    if (add)
    {
        position += motionState->getBTTransform()->getOrigin();
    }

    updateBody(phShape, mass, position, motionState->getBTTransform()->getRotation());
}

void PhysicsObject::setRotation(btQuaternion rotation, bool add)
{
    if (add)
    {
        rotation *= motionState->getBTTransform()->getRotation();
    }

    updateBody(phShape, mass, motionState->getBTTransform()->getOrigin(), rotation);
}
        
void PhysicsObject::setTransform(btScalar* transform)
{
    motionState->setGLTransform(transform);
    body->setWorldTransform(*motionState->getBTTransform());     
}

void PhysicsObject::setTransform(btTransform* transform)
{
    motionState->setBTTransform(transform);
    body->setWorldTransform(*motionState->getBTTransform());     
}

void PhysicsObject::clearTransform()
{
    updateBody(phShape, mass, btVector3(0, 0, 0), btQuaternion(btVector3(0, 1, 0), 0));
}

void PhysicsObject::setCollidable(bool collidable)
{
    if (this->collidable != collidable)
    {
        this->collidable = collidable;

        if (body)
        {
            if (collidable)
            {
                body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);
            }
            else
            {
                body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);
            }
        }
    }
}

void PhysicsObject::setStatic(bool stat)
{
    if (this->stat != stat)
    {
        this->stat = stat;

        if (body)
        {
            if (stat)
            {
                body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CollisionFlags::CF_STATIC_OBJECT);
            }
            else
            {
                body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CollisionFlags::CF_STATIC_OBJECT);
            }
        }
    }
}

void PhysicsObject::setUserPointer(void* userPointer)
{
    this->userPointer = userPointer;
}

float PhysicsObject::getMass() const
{
    return mass;
}

btRigidBody* PhysicsObject::getRigidBody() const
{
    return body;
}

btCollisionShape* PhysicsObject::getShape() const
{
    return phShape;
}

CompoundShape* PhysicsObject::getCompoundShape() const
{
    return comShape;
}

bool PhysicsObject::isCollidable() const
{
    return collidable;
}

bool PhysicsObject::isStatic() const
{
    return stat;
}

void* PhysicsObject::getUserPointer() const
{
    return userPointer;
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
