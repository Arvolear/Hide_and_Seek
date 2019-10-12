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

        set < pair < btRigidBody*, btRigidBody* > > touchingBodies;
        set < pair < btRigidBody*, btRigidBody* > > collidedBodies;
        set < pair < btRigidBody*, btRigidBody* > > separatedBodies;

        void collisionEvent(btRigidBody* body0, btRigidBody* body1) override;
        void separationEvent(btRigidBody* body0, btRigidBody* body1) override;

        void clearEventsData();
        
        DebugDrawer* debugDrawer;

    public: 
        World();

        void pollEvents(); 
        bool isTouching(btRigidBody* body0, btRigidBody* body1) const;
        bool isCollided(btRigidBody* body0, btRigidBody* body1) const;
        bool isSeparated(btRigidBody* body0, btRigidBody* body1) const;

        set < btRigidBody* > getTouchingWith(btRigidBody* body0) const;
        set < btRigidBody* > getCollidedWith(btRigidBody* body0) const;
        set < btRigidBody* > getSeparatedWith(btRigidBody* body0) const;

        void updateSimulation(float dt, int subSteps = 1, float fixedStep = 1.0 / 60.0);
       
        btDynamicsWorld* getWorld() const;

        /*** DEBUG ***/
        void createDebugDrawer();
        void renderDebug();
        DebugDrawer* getDebugDrawer() const;

        ~World();
};
