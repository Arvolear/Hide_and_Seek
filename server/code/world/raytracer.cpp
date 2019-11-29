#include "raytracer.hpp"

RayTracer::RayTracer(btDynamicsWorld* world)
{
    this->world = world; // pointer to the physics world
}

RayResult* RayTracer::rayCast(btRigidBody* me, btVector3 rayFrom, btVector3 rayTo, bool missStatic, bool missKinematic) const
{
    if (!world)
    {
        return nullptr;
    }

    btVector3 rT = rayTo; 

    rT *= 10000; // rayTo is really small 

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rT);

    world->rayTest(rayFrom, rT, rayCallback); // perform ray test in bullet 
   
    if (rayCallback.hasHit())
    {
        btRigidBody *body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject); // from const to normal

        if (!body)
        {
            return nullptr;
        }

        if ((body == me) || (body->isStaticObject() && missStatic) || (body->isKinematicObject() && missKinematic))
        {
            return rayCast(me, rayCallback.m_hitPointWorld, rayTo, missStatic, missKinematic); 
        }

        RayResult* result = new RayResult();
        result->body = body; // set hit body
        result->hitPoint = rayCallback.m_hitPointWorld; // set hit point

        return result;
    }

    return nullptr;
}
        
RayTracer::~RayTracer() {}
