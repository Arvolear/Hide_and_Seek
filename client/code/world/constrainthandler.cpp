#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "raytracer.hpp"
#include "constrainthandler.hpp"

ConstraintHandler::ConstraintHandler(btDynamicsWorld *world)
{
    this->world = world;

    camera = nullptr;
    projection = mat4(1.0);
    
    rayTracer = new RayTracer(world);
}

ConstraintHandler::ConstraintHandler(btDynamicsWorld *world, Camera* camera, mat4 &projection)
{
    this->world = world;

    this->camera = camera;
    this->projection = projection;
    
    rayTracer = new RayTracer(world, camera, projection);
}

void ConstraintHandler::setCamera(Camera* camera)
{
    this->camera = camera;
    rayTracer->setCamera(camera);
}

void ConstraintHandler::setProjection(mat4 &projection)
{
    this->projection = projection;
    rayTracer->setProjection(projection);
}

void ConstraintHandler::create(float posx, float posy, bool pickStatic)
{
    if (!world || !camera) 
    {
        return;
    }

    vec3 cWorld = camera->getPosition();
    btVector3 cameraWorld = btVector3(cWorld.x, cWorld.y, cWorld.z);

    btVector3 rayFromCameraToCursor = rayTracer->getPickingRay(posx, posy);

    RayResult* result = rayTracer->rayCast(cameraWorld, rayFromCameraToCursor, !pickStatic);
    
    if (!result)
    {
        return;
    }

    pickedObject.body = result->body;
    pickedObject.body->setActivationState(DISABLE_DEACTIVATION);

    btVector3 localPivot = pickedObject.body->getCenterOfMassTransform().inverse() * result->hitPoint; // get ray hit point in local coods

    btTransform pivot;
    pivot.setIdentity();
    pivot.setOrigin(localPivot);

    btGeneric6DofConstraint *dof6 = new btGeneric6DofConstraint(*pickedObject.body, pivot, true); // make one-object constraint

    bool limitAngularMotion = false;

    if (limitAngularMotion)
    {
        dof6->setAngularLowerLimit(btVector3(0, 0, 0));
        dof6->setAngularUpperLimit(btVector3(0, 0, 0));
    }

    float cfm = 0.1f; // spring coefficient
    dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 0);
    dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 1);
    dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 2);
    dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 3);
    dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 4);
    dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 5);

    float erp = 0.4f; // respond coefficient
    dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 0);
    dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 1);
    dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 2);
    dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 3);
    dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 4);
    dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 5);

    pickedObject.distance = (result->hitPoint - cameraWorld).length();

    //cout << pickedObject.distance << endl;
    
    pickedObject.constraint = dof6;
    world->addConstraint(dof6, true);

    delete result;
}

void ConstraintHandler::update(float posx, float posy)
{
    if (!pickedObject.constraint || !world || !camera)
    {
        return;
    }

    btGeneric6DofConstraint* pickCons = static_cast < btGeneric6DofConstraint* > (pickedObject.constraint); 

    if (!pickCons)
    {
        return;
    }

    vec3 cWorld = camera->getPosition();
    btVector3 cameraWorld = btVector3(cWorld.x, cWorld.y, cWorld.z);
    
    btVector3 ray = rayTracer->getPickingRay(posx, posy); // calculate new ray

    ray *= pickedObject.distance;

    btVector3 newPivot = ray + cameraWorld; // calculate new pivot position

    pickCons->getFrameOffsetA().setOrigin(newPivot); // get A (first) object transform matrix and apply new origin to it
}
        
void ConstraintHandler::remove()
{
    if (!pickedObject.constraint || !world)
    {
        return;
    }

    world->removeConstraint(pickedObject.constraint);

    delete pickedObject.constraint;

    pickedObject.body->forceActivationState(ACTIVE_TAG);
    pickedObject.body->setDeactivationTime(0.0f);
    
    pickedObject.body = nullptr; 
    pickedObject.constraint = nullptr;
}
        
RayTracer* ConstraintHandler::getRayTracer() const
{
    return rayTracer;
}

ConstraintHandler::~ConstraintHandler()
{
    remove();

    delete rayTracer;
}
