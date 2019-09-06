#pragma once

//native
#include <iostream>
#include <vector>
#include <set>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

class CompoundShape
{
    private:
        btCompoundShape* shape;
        vector < btCollisionShape* > childShapes;
    
    public:
        CompoundShape();

        void setShape(btCompoundShape* shape);

        void add(btCollisionShape* childShape, btVector3 position, btQuaternion rotation = btQuaternion(btVector3(0, 0, 1), 0));

        btCompoundShape* getShape() const;
        btCollisionShape* getChildShape(unsigned int index) const;
        btTransform getChildTransform(unsigned int index) const;

        ~CompoundShape();
};

class PhysicsObject
{
    protected:
        static set < string > globalNames;
        string name;

        int ownerID;

        btDynamicsWorld* world;

        bool collidable;

        float mass;
        btCollisionShape* phShape;
        CompoundShape* comShape;
        btRigidBody* body;

        OpenGLMotionState* motionState;

        void* userPointer;

        void updateBody(btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation);

    public:
        PhysicsObject(string name, btDynamicsWorld* world);
        PhysicsObject(string name, btDynamicsWorld* world, btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation = btQuaternion(btVector3(0, 0, 1), 0));
        PhysicsObject(string name, btDynamicsWorld* world, CompoundShape* shape, float mass, btVector3 position, btQuaternion rotation = btQuaternion(btVector3(0, 0, 1), 0));

        void setName(string name);
        void setOwnerID(int ownerID);
        void setShape(btCollisionShape* shape);
        void setShape(CompoundShape* shape);
        void setMass(float mass, bool add = true);
        void setPosition(btVector3 position, bool add = true);
        void setRotation(btQuaternion rotation, bool add = true);
        void setTransform(btTransform* transform);
        void setTransform(btScalar* transform);
        void clearTransform();
        void setCollidable(bool collidable);

        void setUserPointer(void* userPointer);

        string getName() const;
        int getOwnerID() const;
        float getMass() const;
        btCollisionShape* getShape() const;
        CompoundShape* getCompoundShape() const;
        btRigidBody* getRigidBody() const;
        bool isCollidable() const;

        void* getUserPointer() const;

        btScalar* getTransform() const;

        virtual ~PhysicsObject();
};
