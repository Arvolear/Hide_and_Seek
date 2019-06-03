#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "../debug/debugdrawer.hpp"

#include "raytracer.hpp"
#include "constrainthandler.hpp"
#include "bulletevents.hpp"
#include "world.hpp"

using namespace std;

World::World() : BulletEvents()
{
    broadphase = new btDbvtBroadphase(); // create the broadphase
    collisionConfiguration = new btDefaultCollisionConfiguration();// create the collision configuration
    dispatcher = new btCollisionDispatcher(collisionConfiguration); // create the dispatcher
    solver = new btSequentialImpulseConstraintSolver(); // create the constraint solver
    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration); //create the world

    debugDrawer = nullptr;
}

void World::collisionEvent(btRigidBody* body0, btRigidBody* body1)
{
    collidedBodies.insert({body0, body1}); 
}

void World::separationEvent(btRigidBody* body0, btRigidBody* body1)
{
    separatedBodies.insert({body0, body1}); 
}

void World::clearEventsData()
{
    collidedBodies.clear();
    separatedBodies.clear();
}

void World::pollEvents()
{
    checkForCollisionEvents(dispatcher);
}

bool World::isCollided(btRigidBody* body0, btRigidBody* body1) const
{
    if (collidedBodies.find({body0, body1}) == collidedBodies.end()) 
    {
        return false;
    }

    return true;
}

bool World::isSeparated(btRigidBody* body0, btRigidBody* body1) const
{
    if (separatedBodies.find({body0, body1}) == separatedBodies.end()) 
    {
        return false;
    }

    return true;
}
        
void World::updateSimulation(float dt)
{
    clearEventsData();

    if (world)
    {
        world->stepSimulation(dt, 2);
    }
}
        
btDynamicsWorld* World::getWorld() const
{
    return world;
}

/*** DEBUG ***/
void World::createDebugDrawer()
{
    debugDrawer = new DebugDrawer();
    world->setDebugDrawer(debugDrawer);
}

void World::renderDebug()
{
    world->debugDrawWorld();
}

DebugDrawer* World::getDebugDrawer() const
{
    return debugDrawer;
}

World::~World()
{
    delete world;
    delete broadphase; 
    delete collisionConfiguration;
    delete dispatcher;
    delete solver;

    delete debugDrawer;
}
