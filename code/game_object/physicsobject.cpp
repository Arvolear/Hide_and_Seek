#include "openglmotionstate.hpp"
#include "physicsobject.hpp"

using namespace std;

PhysicsObject::PhysicsObject(btCollisionShape *shape, float mass, const btVector3 &initialPosition, const btQuaternion &initialRotation)
{
    m_Shape = shape;

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(initialPosition);
    transform.setRotation(initialRotation);

    m_MotionState = new OpenGLMotionState(transform);

    btVector3 localInertia(0, 0, 0);

    if (mass)
    {
        m_Shape->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_MotionState, m_Shape, localInertia);

    m_Body = new btRigidBody(cInfo);
}

PhysicsObject::PhysicsObject(vector < PhysicsObjectCompound* > &compoundInfo, float mass, const btVector3 &initialPosition, const btQuaternion &initialRotation, bool clearVector)
{
    m_Compound = new btCompoundShape();

    for (size_t i = 0; i < compoundInfo.size(); i++)
    {
        m_Shapes.push_back(compoundInfo[i]->bodyShape);


        btTransform localTransform;

        localTransform.setIdentity();
        localTransform.setOrigin(compoundInfo[i]->position);
        localTransform.setRotation(compoundInfo[i]->rotation);
        m_Compound->addChildShape(localTransform, compoundInfo[i]->bodyShape);
    }

    m_Shape = m_Compound;
    
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(initialPosition);
    transform.setRotation(initialRotation);

    m_MotionState = new OpenGLMotionState(transform);

    btVector3 localInertia(0, 0, 0);

    if (mass)
    {
        m_Shape->calculateLocalInertia(mass, localInertia);
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_MotionState, m_Compound, localInertia);

    m_Body = new btRigidBody(cInfo);

    if (clearVector)
    {
        for (size_t i = 0; i < compoundInfo.size(); i++)
        {
            delete compoundInfo[i];
        }

        compoundInfo.clear();
    }
}

void PhysicsObject::setIndex(int index)
{
    this->index = index;
}

void PhysicsObject::removeIndex()
{
    index = -1;
}

btRigidBody* PhysicsObject::getRigidBody() const
{
    return m_Body;
}

btMotionState* PhysicsObject::getMotionState() const
{
    return m_MotionState;
}

void PhysicsObject::getTransform(btScalar* transform) const
{
    m_MotionState->getTransform(transform);
}

unsigned long int PhysicsObject::getIndex() const
{
    return index;
}

PhysicsObject::~PhysicsObject()
{
    delete m_Shape;
    //delete m_Compound;

    for (size_t i = 0; i < m_Shapes.size(); i++)
    {
        delete m_Shapes[i];
    }

    delete m_Body;

    delete m_MotionState;
}
