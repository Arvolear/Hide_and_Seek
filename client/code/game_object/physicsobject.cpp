#include "../shader/shader.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "../debug/debugdrawer.hpp"

#include "../world/raytracer.hpp"
#include "../world/constrainthandler.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "openglmotionstate.hpp"
#include "physicsobject.hpp"

/********* COMPOUND SHAPE **********/
        
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

vector < btCollisionShape* > CompoundShape::getChildrenShapes() const
{
    return childShapes;
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

/********* CONVEX HULL SHAPE **********/

ConvexHullShape::ConvexHullShape()
{
    shape = new btConvexHullShape();
}

void ConvexHullShape::setShape(btConvexHullShape* shape)
{
    this->shape = shape;
}

void ConvexHullShape::addPoint(btVector3 point)
{
    shape->addPoint(point);
}

btConvexHullShape* ConvexHullShape::getShape() const
{
    return shape;
}

btVector3 ConvexHullShape::getPoint(unsigned int index) const
{
    if (index < 0 || index >= points.size())
    {
        throw range_error("ERROR convexhull shape has no " + to_string(index) + " point");
    }
    
    return points[index];
}

vector < btVector3 > ConvexHullShape::getPoints() const
{
    return points;
}

ConvexHullShape::~ConvexHullShape() {}

/********* PHYSICS OBJECT **********/

void PhysicsObject::updateBody(btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation)
{
    if (!shape)
    {
        return;
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

    btTransform transform = motionState->getBTTransform();

    transform.setIdentity();
    transform.setOrigin(position);
    transform.setRotation(rotation);

    motionState->setBTTransform(transform);
    motionState->update();
    
    body->setWorldTransform(motionState->getBTTransform());     

    body->setCollisionShape(phShape);
    body->setMassProps(mass, localInertia); 
    
    physicsWorld->getWorld()->removeRigidBody(body);
    physicsWorld->getWorld()->addRigidBody(body);

    //body->forceActivationState(ACTIVE_TAG); // server is responsible for this
}        

PhysicsObject::PhysicsObject(World* physicsWorld)
{
    this->physicsWorld = physicsWorld;
    this->mass = 0;
    this->phShape = nullptr;
    this->conShape = nullptr;
    this->comShape = nullptr;
    this->body = nullptr;
    this->collidable = true;
    this->stat = true;
    this->userPointer = nullptr;

    btTransform transform;
    transform.setIdentity();

    motionState = new OpenGLMotionState(transform);
    body = new btRigidBody(mass, motionState, nullptr);
    body->setUserPointer(this);
}

PhysicsObject::PhysicsObject(World* physicsWorld, btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation)
{
    this->physicsWorld = physicsWorld;
    this->mass = mass;
    this->body = nullptr;
    this->conShape = nullptr;
    this->comShape = nullptr;
    this->collidable = true;
    this->stat = mass ? false : true;
    this->userPointer = nullptr;

    btTransform transform;
    transform.setIdentity();

    motionState = new OpenGLMotionState(transform);
    body = new btRigidBody(mass, motionState, nullptr);
    body->setUserPointer(this);
    
    updateBody(shape, mass, position, rotation);
}

PhysicsObject::PhysicsObject(World* physicsWorld, ConvexHullShape* shape, float mass, btVector3 position, btQuaternion rotation)
{
    this->physicsWorld = physicsWorld;
    this->mass = mass;
    this->body = nullptr;
    this->conShape = nullptr;
    this->comShape = nullptr;
    this->collidable = true;
    this->stat = mass ? false : true;
    this->userPointer = nullptr;

    btTransform transform;
    transform.setIdentity();

    motionState = new OpenGLMotionState(transform);
    body = new btRigidBody(mass, motionState, nullptr);
    body->setUserPointer(this);
    
    this->conShape = shape;
    
    updateBody(shape->getShape(), mass, position, rotation);
}

PhysicsObject::PhysicsObject(World* physicsWorld, CompoundShape* shape, float mass, btVector3 position, btQuaternion rotation)
{
    this->physicsWorld = physicsWorld;
    this->mass = mass;
    this->body = nullptr;
    this->conShape = nullptr;
    this->comShape = nullptr;
    this->collidable = true;
    this->stat = mass ? false : true;
    this->userPointer = nullptr;

    btTransform transform;
    transform.setIdentity();

    motionState = new OpenGLMotionState(transform);
    body = new btRigidBody(mass, motionState, nullptr);
    body->setUserPointer(this);
    
    this->comShape = shape;
    
    updateBody(shape->getShape(), mass, position, rotation);
}

void PhysicsObject::setShape(btCollisionShape* shape)
{
    delete this->phShape;
    this->phShape = nullptr;
    delete this->conShape;
    this->conShape = nullptr;
    delete this->comShape;
    this->comShape = nullptr;

    updateBody(shape, mass, motionState->getBTTransform().getOrigin(), motionState->getBTTransform().getRotation());
}

void PhysicsObject::setShape(ConvexHullShape* shape)
{
    delete this->phShape;
    this->phShape = nullptr;
    delete this->conShape;
    this->conShape = shape;
    delete this->comShape;
    this->comShape = nullptr;
    
    updateBody(shape->getShape(), mass, motionState->getBTTransform().getOrigin(), motionState->getBTTransform().getRotation());
}

void PhysicsObject::setShape(CompoundShape* shape)
{
    delete this->phShape;
    this->phShape = nullptr;
    delete this->conShape;
    this->conShape = nullptr;
    delete this->comShape;
    this->comShape = shape;
    
    updateBody(shape->getShape(), mass, motionState->getBTTransform().getOrigin(), motionState->getBTTransform().getRotation());
}

void PhysicsObject::setMass(float mass, bool add)
{
    if (add)
    {
        mass += this->mass;
    }
        
    updateBody(phShape, mass, motionState->getBTTransform().getOrigin(), motionState->getBTTransform().getRotation());
}

void PhysicsObject::setPosition(btVector3 position, bool add)
{
    if (add)
    {
        position += motionState->getBTTransform().getOrigin();
    }

    updateBody(phShape, mass, position, motionState->getBTTransform().getRotation());
}

void PhysicsObject::setRotation(btQuaternion rotation, bool add)
{
    if (add)
    {
        rotation *= motionState->getBTTransform().getRotation();
    }

    updateBody(phShape, mass, motionState->getBTTransform().getOrigin(), rotation);
}
        
void PhysicsObject::setTransform(btScalar* transform)
{
    motionState->setGLTransform(transform);
    body->setWorldTransform(motionState->getBTTransform());
}

void PhysicsObject::setTransform(btTransform transform)
{
    motionState->setBTTransform(transform);
    body->setWorldTransform(motionState->getBTTransform());
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

ConvexHullShape* PhysicsObject::getConvexHullShape() const
{
    return conShape;
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
        physicsWorld->getWorld()->removeRigidBody(body);
    }

    delete phShape;
    delete conShape;
    delete comShape;
    delete body;
    delete motionState;
}
