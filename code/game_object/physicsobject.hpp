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
        btCollisionShape *m_Shape = NULL;
        vector < btCollisionShape* > m_Shapes; 
        btCompoundShape* m_Compound = NULL;

        btRigidBody *m_Body = NULL;

        OpenGLMotionState *m_MotionState = NULL;

    public:
        PhysicsObject(btCollisionShape *Shape, float mass, const btVector3 &initialPosition = btVector3(0, 0, 0), const btQuaternion &initialRotation = btQuaternion(btVector3(0, 0, 0), 0));
        PhysicsObject(vector < PhysicsObjectCompound* > &compoundInfo, float mass, const btVector3 &initialPosition = btVector3(0, 0, 0), const btQuaternion &initialRotation = btQuaternion(btVector3(0, 0, 0), 0), bool clearVector = false);

        btCollisionShape *getShape();
        btRigidBody *getRigidBody();
        btMotionState *getMotionState();
        void getTransform(btScalar *transform);

        ~PhysicsObject();
};
