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
        vector < btCollisionShape* > getChildrenShapes() const;
        btTransform getChildTransform(unsigned int index) const;

        ~CompoundShape();
};

class ConvexHullShape
{
    private:
        btConvexHullShape* shape;
        vector < btVector3 > points;

    public:
        ConvexHullShape();
        
        void setShape(btConvexHullShape* shape);

        void addPoint(btVector3 point);

        btConvexHullShape* getShape() const;
        btVector3 getPoint(unsigned int index) const;
        vector < btVector3 > getPoints() const;

        ~ConvexHullShape();
};

class PhysicsObject
{
    protected:
        static set < string > globalNames;
        string name;

        int ownerID;

        World* physicsWorld;

        bool collidable;
        bool stat;
        bool kinematic;

        float mass;
        btCollisionShape* phShape;
        ConvexHullShape* conShape;
        CompoundShape* comShape;
        btRigidBody* body;

        OpenGLMotionState* motionState;

        void* userPointer;

        void updateBody(btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation, int group = btBroadphaseProxy::DefaultFilter);

    public:
        PhysicsObject(string name, World* physicsWorld);
        PhysicsObject(string name, World* physicsWorld, btCollisionShape* shape, float mass, btVector3 position, btQuaternion rotation = btQuaternion(btVector3(0, 0, 1), 0));
        PhysicsObject(string name, World* physicsWorld, ConvexHullShape* shape, float mass, btVector3 position, btQuaternion rotation = btQuaternion(btVector3(0, 0, 1), 0));
        PhysicsObject(string name, World* physicsWorld, CompoundShape* shape, float mass, btVector3 position, btQuaternion rotation = btQuaternion(btVector3(0, 0, 1), 0));

        void setName(string name);
        void setOwnerID(int ownerID);
        void setShape(btCollisionShape* shape);
        void setShape(ConvexHullShape* shape);
        void setShape(CompoundShape* shape);
        void setMass(float mass, bool add = true);
        void setPosition(btVector3 position, bool add = true);
        void setRotation(btQuaternion rotation, bool add = true);
        void setTransform(btTransform transform);
        void setTransform(btScalar* transform);
        void clearTransform();
        void setCollidable(bool collidable);
        void setStatic(bool stat);
        void setKinematic(bool kinematic);

        void setUserPointer(void* userPointer);

        string getName() const;
        int getOwnerID() const;
        float getMass() const;
        btCollisionShape* getShape() const;
        ConvexHullShape* getConvexHullShape() const;
        CompoundShape* getCompoundShape() const;
        btRigidBody* getRigidBody() const;
        bool isCollidable() const;
        bool isStatic() const; 
        bool isKinematic() const;

        void* getUserPointer() const;

        btScalar* getTransform() const;

        virtual ~PhysicsObject();
};
