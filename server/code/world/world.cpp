#include "raytracer.hpp"
#include "bulletevents.hpp"
#include "world.hpp"

using namespace std;

World::World() : BulletEvents()
{
    broadphase = new btDbvtBroadphase(); // create the broadphase
    collisionConfiguration = new btDefaultCollisionConfiguration();// create the collision configuration
    dispatcher = new btCollisionDispatcher(collisionConfiguration); // create the dispatcher
    solver = new btSequentialImpulseConstraintSolver(); // create the constraint solver
    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration); // create the world
}

void World::collisionEvent(btRigidBody* body0, btRigidBody* body1)
{
    collidedBodies.insert({body0, body1});

    if (touchingBodies.find({body0, body1}) == touchingBodies.end() && touchingBodies.find({body1, body0}) == touchingBodies.end())
    {
        touchingBodies.insert({body0, body1});
    }
}

void World::separationEvent(btRigidBody* body0, btRigidBody* body1)
{
    separatedBodies.insert({body0, body1}); 
    
    if (touchingBodies.find({body0, body1}) != touchingBodies.end())
    {
        touchingBodies.erase({body0, body1});
    }
    
    if (touchingBodies.find({body1, body0}) != touchingBodies.end())
    {
        touchingBodies.erase({body1, body0});
    }
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

bool World::isTouching(btRigidBody* body0, btRigidBody* body1) const
{
    if (touchingBodies.find({body0, body1}) == touchingBodies.end() && touchingBodies.find({body1, body0}) == touchingBodies.end()) 
    {
        return false;
    }

    return true;
}

bool World::isCollided(btRigidBody* body0, btRigidBody* body1) const
{
    if (collidedBodies.find({body0, body1}) == collidedBodies.end() && collidedBodies.find({body1, body0}) == collidedBodies.end()) 
    {
        return false;
    }

    return true;
}

bool World::isSeparated(btRigidBody* body0, btRigidBody* body1) const
{
    if (separatedBodies.find({body0, body1}) == separatedBodies.end() && separatedBodies.find({body1, body0}) == separatedBodies.end()) 
    {
        return false;
    }

    return true;
}

set < btRigidBody* > World::getTouchingWith(btRigidBody* body0) const
{
    set < btRigidBody* > res;

    for (auto& i: touchingBodies)
    {
        if (i.first == body0)
        {
            res.insert(i.second);
        }
        else if (i.second == body0)
        {
            res.insert(i.first);
        }
    }

    /* move constructor */
    return move(res);
}

set < btRigidBody* > World::getCollidedWith(btRigidBody* body0) const
{
    set < btRigidBody* > res;

    for (auto& i: collidedBodies)
    {
        if (i.first == body0)
        {
            res.insert(i.second);
        }
        else if (i.second == body0)
        {
            res.insert(i.first);
        }
    }

    /* move constructor */
    return move(res);
}

set < btRigidBody* > World::getSeparatedWith(btRigidBody* body0) const
{
    set < btRigidBody* > res;

    for (auto& i: separatedBodies)
    {
        if (i.first == body0)
        {
            res.insert(i.second);
        }
        else if (i.second == body0)
        {
            res.insert(i.first);
        }
    }

    /* move constructor */
    return move(res);
}
        
void World::updateSimulation(float dt)
{
    clearEventsData();

    if (world)
    {
        world->stepSimulation(dt, 1);
    }
}
        
btDynamicsWorld* World::getWorld() const
{
    return world;
}

World::~World()
{
    delete world;
    delete broadphase; 
    delete collisionConfiguration;
    delete dispatcher;
    delete solver;
}
