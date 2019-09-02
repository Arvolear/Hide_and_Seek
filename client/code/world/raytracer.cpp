#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "raytracer.hpp"

RayTracer::RayTracer(btDynamicsWorld* world)
{
    this->world = world; // pointer to our physics world
    camera = nullptr;
    projection = mat4(1.0);
}

RayTracer::RayTracer(btDynamicsWorld* world, Camera* camera, mat4 &projection)
{
    this->world = world; // pointer to our physics world
    this->camera = camera;
    this->projection = projection;
}

void RayTracer::setCamera(Camera* camera)
{
    this->camera = camera;
}

void RayTracer::setProjection(mat4 &projection)
{
    this->projection = projection;
}        

btVector3 RayTracer::getPickingRay(double posx, double posy) const
{
    if (!camera)
    {
        return btVector3(0, 0, 0);
    }

    vec2 windowSize = camera->getWindowSize();

    vec2 mouseClip = vec2(posx * 2 / windowSize.x - 1, 1 - posy * 2 / windowSize.y); // calculate mouse clip coords [-1;1] from window coords

    vec3 cameraWorld = camera->getPosition();
    mat4 cameraTranslate = camera->getView(); // get view matrix

    vec4 mouseWorld = inverse(cameraTranslate) * inverse(projection) * vec4(mouseClip, 0, 1); // calculate mouse position on view near plane 

    mouseWorld /= mouseWorld.w; // normalize

    vec3 ray = normalize(vec3(mouseWorld) - cameraWorld); // calculate ray direction
    
    //cout << "Ray " << ray.x << ' ' << ray.y << ' ' << ray.z << endl << endl;

    return btVector3(ray.x, ray.y, ray.z);
}
        
RayResult* RayTracer::rayCast(btVector3 &rayFrom, btVector3 &rayTo, bool missStatic) const
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

        if ((body->isStaticObject() || body->isKinematicObject()) && missStatic)
        {
            return nullptr;
        }

        RayResult* result = new RayResult();
        result->body = body; // set hit body
        result->hitPoint = rayCallback.m_hitPointWorld; // set hit point

        return result;
    }

    return nullptr;
}
        
RayTracer::~RayTracer() {}
