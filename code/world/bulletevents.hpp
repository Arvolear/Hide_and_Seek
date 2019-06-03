#pragma once

//native
#include <set>
#include <algorithm>
#include <iterator>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

class BulletEvents
{
    protected:
        set < pair < const btRigidBody*, const btRigidBody* > > m_PairsLastUpdate;

        void checkForCollisionEvents(btCollisionDispatcher *m_Dispatcher);

    public:
        BulletEvents();

        virtual void collisionEvent(btRigidBody *body0, btRigidBody *body1) = 0;
        virtual void separationEvent(btRigidBody *body0, btRigidBody *body1) = 0;

        virtual ~BulletEvents();
};
