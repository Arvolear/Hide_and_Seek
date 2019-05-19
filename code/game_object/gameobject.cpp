#include "../global/convert.hpp"
#include "../global/shader.hpp"
#include "../debug/debugsphere.hpp"
#include "openglmotionstate.hpp"
#include "animation.hpp"
#include "mesh.hpp"
#include "bone.hpp"
#include "skeleton.hpp"
#include "viewfrustum.hpp"
#include "boundsphere.hpp"
#include "modelloader.hpp"
#include "physicsobject.hpp"
#include "gameobject.hpp"

GameObject::GameObject()
{
    world = nullptr;
    modelLoader = new ModelLoader();
    physicsObject = nullptr;

    skeleton = nullptr;
    viewFrustum = nullptr;
    boundSphere = nullptr;
    debugSphere = nullptr;
}

GameObject::GameObject(string path, PhysicsObject* physicsObject, btDynamicsWorld* world, ViewFrustum* viewFrustum)
{
    this->world = world;
    this->physicsObject = physicsObject;
    this->viewFrustum = viewFrustum;
    this->graphicsObject = path;

    modelLoader = new ModelLoader();
    modelLoader->loadModel(path);
    
    modelLoader->getModelData(skeleton, meshes);
    
    boundSphere = new BoundSphere(meshes);
    boundSphere->construct();
}

void GameObject::removePhysicsObject()
{
    if (physicsObject)
    {
        world->removeRigidBody(physicsObject->getRigidBody());

        delete physicsObject;

        physicsObject = nullptr;
    }
}

void GameObject::removeGraphicsObject()
{
    for (size_t i = 0; i < meshes.size(); i++)
    {
        delete meshes[i];
    }
    meshes.clear();

    delete skeleton;
    skeleton = nullptr;

    delete boundSphere;

    graphicsObject = "";
    modelLoader->clear();
}

mat4 GameObject::getPhysicsObjectTransform() const
{
    if (!physicsObject)
    {
        return mat4(1.0);
    }

    btScalar transform[16];

    physicsObject->getTransform(transform);

    return btScalar2glmMat4(transform);
}

void GameObject::setGraphicsObject(string path)
{
    removeGraphicsObject();
    graphicsObject = path;

    modelLoader->loadModel(path);
    modelLoader->getModelData(skeleton, meshes);

    boundSphere = new BoundSphere(meshes);
    boundSphere->construct();
}
        
void GameObject::setViewFrustum(ViewFrustum* viewFrustum)
{
    this->viewFrustum = viewFrustum;
}

void GameObject::setPhysicsObject(PhysicsObject* object)
{
    removePhysicsObject();

    physicsObject = object;

    world->addRigidBody(physicsObject->getRigidBody());
}

void GameObject::setLocalRotation(float angle, vec3 axis)
{
    vec3 sc;
    quat rot;
    vec3 tran;
    vec3 skew;
    vec4 perspective;

    decompose(localTransform, sc, rot, tran, skew, perspective);

    localTransform = mat4(1.0);
    localTransform *= scale(sc);
    localTransform *= translate(tran);
    localTransform *= rotate(radians(angle), axis) * toMat4(conjugate(rot));
}

void GameObject::setLocalScale(vec3 growth)
{
    vec3 sc;
    quat rot;
    vec3 tran;
    vec3 skew;
    vec4 perspective;

    decompose(localTransform, sc, rot, tran, skew, perspective);

    localTransform = mat4(1.0);
    localTransform *= scale(growth) * scale(sc);
    localTransform *= translate(tran);
    localTransform *= toMat4(conjugate(rot));
}

void GameObject::setLocalPosition(vec3 translation)
{
    vec3 sc;
    quat rot;
    vec3 tran;
    vec3 skew;
    vec4 perspective;

    decompose(localTransform, sc, rot, tran, skew, perspective);

    localTransform = mat4(1.0);
    localTransform *= scale(sc);
    localTransform *= translate(translation) * translate(tran);
    localTransform *= toMat4(conjugate(rot));
}

void GameObject::playAnimation(Animation* anim, bool reset)
{
    skeleton->playAnimation(anim, reset);
}

void GameObject::stopAnimation()
{
    skeleton->stopAnimation();
}

PhysicsObject* GameObject::getPhysicsObject() const
{
    return physicsObject;
}

string GameObject::getGraphicsObject() const
{
    return graphicsObject;
}

void GameObject::render(Shader* shader, bool check)
{
    if (check && viewFrustum)
    {
        mat4 transform = getPhysicsObjectTransform() * localTransform;

        boundSphere->applyTransform(transform);

        if (!viewFrustum->isSphereInFrustum(boundSphere->getTransformedCenter(), boundSphere->getTransformedRadius()))
        {
            return;
        }
    }

    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "localTransform"), 1, GL_FALSE, value_ptr(localTransform));
    
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, GL_FALSE, value_ptr(getPhysicsObjectTransform()));

    skeleton->update(shader);

    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i]->draw(shader);
    }
}

/************************/

void GameObject::createDebugSphere(int depth)
{
    debugSphere = new DebugSphere();

    if (boundSphere)
    {
        debugSphere->construct(boundSphere->getCenter(), boundSphere->getRadius(), depth);
    }
}

void GameObject::renderDebugSphere(mat4 &view, mat4 &projection, Shader *shader)
{
    if (debugSphere)
    {
        mat4 transform = viewFrustum->getProjection() * viewFrustum->getView() * getPhysicsObjectTransform() * localTransform;
        debugSphere->applyTransform(transform);

        debugSphere->draw(shader);
    }
}

/************************/

GameObject::~GameObject()
{
    delete modelLoader;

    removePhysicsObject();
    removeGraphicsObject();

    delete debugSphere;
}
