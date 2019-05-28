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
        unsigned long int index = -1;

        btCollisionShape *m_Shape = nullptr;
        vector < btCollisionShape* > m_Shapes; 
        btCompoundShape* m_Compound = nullptr;

        btRigidBody *m_Body = nullptr;

        OpenGLMotionState *m_MotionState = nullptr;

    public:
        PhysicsObject(btCollisionShape *Shape, float mass, const btVector3 &initialPosition = btVector3(0, 0, 0), const btQuaternion &initialRotation = btQuaternion(btVector3(0, 0, 0), 0));
        PhysicsObject(vector < PhysicsObjectCompound* > &compoundInfo, float mass, const btVector3 &initialPosition = btVector3(0, 0, 0), const btQuaternion &initialRotation = btQuaternion(btVector3(0, 0, 0), 0), bool clearVector = true);

        void setIndex(int index);
        void removeIndex();

        btRigidBody* getRigidBody() const;
        btMotionState* getMotionState() const;
        void getTransform(btScalar *transform) const;
        unsigned long int getIndex() const;

        ~PhysicsObject();
};
