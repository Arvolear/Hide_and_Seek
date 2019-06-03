#include "bulletevents.hpp"

using namespace std;

BulletEvents::BulletEvents() {}

void BulletEvents::checkForCollisionEvents(btCollisionDispatcher *m_Dispatcher)
{
    set < pair < const btRigidBody*, const btRigidBody* > > m_PairsThisUpdate;

    for (int i = 0; i < m_Dispatcher->getNumManifolds(); i++)
    {
        btPersistentManifold *manifold = m_Dispatcher->getManifoldByIndexInternal(i);

        if (manifold->getNumContacts() > 0)
        {
            const btRigidBody *body0 = static_cast < const btRigidBody* > (manifold->getBody0()); 
            const btRigidBody *body1 = static_cast < const btRigidBody* > (manifold->getBody1());

            if (body0 > body1)
            {
                swap(body0, body1);
            }

            pair < const btRigidBody*, const btRigidBody* > thisPair = {body0, body1};
            m_PairsThisUpdate.insert(thisPair);

            if (m_PairsLastUpdate.find(thisPair) == m_PairsLastUpdate.end())
            {
                collisionEvent((btRigidBody*)body0, (btRigidBody*)body1);
            }
        }
    }

    set < pair < const btRigidBody*, const btRigidBody* > > m_PairsRemoved;

    set_difference(m_PairsLastUpdate.begin(), m_PairsLastUpdate.end(), m_PairsThisUpdate.begin(), m_PairsThisUpdate.end(), inserter(m_PairsRemoved, m_PairsRemoved.begin()));

    for (auto i: m_PairsRemoved)
    {
        separationEvent((btRigidBody*)i.first, (btRigidBody*)i.second);
    }

    m_PairsLastUpdate = m_PairsThisUpdate;
}

BulletEvents::~BulletEvents() {}
