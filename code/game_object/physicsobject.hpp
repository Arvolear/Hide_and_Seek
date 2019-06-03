#pragma once

//native
#include <vector>

//bullet
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

struct PhysicsObjectCompound
{
    btCollisionShape* bodyShape = NULL;
    btVector3 position = btVector3(0.0f, 0.0f, 0.0f);
    btQuaternion rotation = btQuaternion(btVector3(0, 0, 0), 0);
};

class PhysicsObject
{
    private:
        btCollisionShape* shape = nullptr;
        vector < btCollisionShape* > shapes; 
        btCompoundShape* compound = nullptr;

        btRigidBody* body = nullptr;

        OpenGLMotionState* motionState = nullptr;

    public:
        PhysicsObject(btCollisionShape *Shape, float mass, const btVector3 &initialPosition = btVector3(0, 0, 0), const btQuaternion &initialRotation = btQuaternion(btVector3(0, 0, 0), 0));
        PhysicsObject(vector < PhysicsObjectCompound* > &compoundInfo, float mass, const btVector3 &initialPosition = btVector3(0, 0, 0), const btQuaternion &initialRotation = btQuaternion(btVector3(0, 0, 0), 0), bool clearVector = true);

        btRigidBody* getRigidBody() const;
        btMotionState* getMotionState() const;
        void getTransform(btScalar* transform) const;

        ~PhysicsObject();
};
