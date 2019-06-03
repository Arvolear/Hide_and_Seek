#pragma once

//native
#include <vector>
#include <algorithm>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

//opengl
#include <glm/glm.hpp>

using namespace std;

class World : public BulletEvents
{
    private:
        btBroadphaseInterface* broadphase;
        btCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btConstraintSolver* solver;
        btDynamicsWorld* world;

        set < pair < btRigidBody*, btRigidBody* > > collidedBodies;
        set < pair < btRigidBody*, btRigidBody* > > separatedBodies;

        void collisionEvent(btRigidBody* body0, btRigidBody* body1) override;
        void separationEvent(btRigidBody* body0, btRigidBody* body1) override;

        void clearEventsData();
        
        DebugDrawer* debugDrawer;

    public: 
        World();

        void pollEvents(); 
        bool isCollided(btRigidBody* body0, btRigidBody* body1) const;
        bool isSeparated(btRigidBody* body0, btRigidBody* body1) const;

        void updateSimulation(float dt);
       
        btDynamicsWorld* getWorld() const;

        /*** DEBUG ***/
        void createDebugDrawer();
        void renderDebug();
        DebugDrawer* getDebugDrawer() const;

        ~World();
};
