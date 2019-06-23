#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../debug/debugsphere.hpp"
#include "../debug/debugdrawer.hpp"

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

set < string > GameObject::globalNames;

GameObject::GameObject(string name)
{
    if (globalNames.find(name) != globalNames.end())
    {
        throw runtime_error("ERROR::GameObject invalid name");
    }

    globalNames.insert(name);
    this->name = name;

    modelLoader = new ModelLoader();
    physicsObject = nullptr;

    skeleton = nullptr;
    viewFrustum = nullptr;
    boundSphere = nullptr;
    debugSphere = nullptr;
}

GameObject::GameObject(string name, string path, PhysicsObject* physicsObject, ViewFrustum* viewFrustum)
{
    if (globalNames.find(name) != globalNames.end())
    {
        throw runtime_error("ERROR::GameObject invalid name");
    }

    globalNames.insert(name);
    this->name = name;

    this->physicsObject = physicsObject;
    this->viewFrustum = viewFrustum;
    this->graphicsObject = path;

    modelLoader = new ModelLoader();
    modelLoader->loadModel(path);
    
    modelLoader->getModelData(skeleton, meshes);

    boundSphere = new BoundSphere(meshes);
    boundSphere->construct();

    debugSphere = nullptr;
}

void GameObject::removePhysicsObject()
{
    if (physicsObject)
    {
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
    boundSphere = nullptr;

    graphicsObject = "";
    modelLoader->clear();
}

mat4 GameObject::getPhysicsObjectTransform() const
{
    if (!physicsObject)
    {
        return mat4(1.0);
    }

    /* 16 elements */
    btScalar* transform;
    transform = physicsObject->getTransform();

    mat4 ret = btScalar2glmMat4(transform);
    delete transform;

    return ret;
}

void GameObject::setName(string name)
{
    if (globalNames.find(name) != globalNames.end())
    {
        throw runtime_error("ERROR::GameObject invalid name");
    }

    globalNames.erase(this->name);
    globalNames.insert(name);
    this->name = name;
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
}

void GameObject::setLocalRotation(vec3 axis, float angle)
{
    vec3 sc;
    quat rot;
    vec3 tran;
    vec3 skew;
    vec4 perspective;

    decompose(localTransform, sc, rot, tran, skew, perspective);

    localTransform = mat4(1.0);
    localTransform *= translate(tran);
    localTransform *= scale(sc);
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
    localTransform *= translate(tran);
    localTransform *= scale(growth) * scale(sc);
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
    localTransform *= translate(translation) * translate(tran);
    localTransform *= scale(sc);
    localTransform *= toMat4(conjugate(rot));
}

void GameObject::addAnimation(Animation* anim)
{
    if (animations.find(anim->getName()) == animations.end())
    {
        animations.insert({anim->getName(), anim});
    }
}

void GameObject::playAnimation(string name, bool reset)
{
    if (animations.find(name) != animations.end())
    {
        skeleton->playAnimation(animations.find(name)->second, reset);
    }
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

string GameObject::getName() const
{
    return name;
}
        
Animation* GameObject::getActiveAnimation() const
{
    return skeleton->getAnimation();
}

Animation* GameObject::getAnimation(string name) const
{
    if (animations.find(name) != animations.end())
    {
        return animations.find(name)->second;
    }

    return nullptr;
}

void GameObject::render(Shader* shader, bool cull)
{
    if (cull && viewFrustum && boundSphere)
    {
        mat4 transform = getPhysicsObjectTransform() * localTransform;

        boundSphere->applyTransform(transform);

        if (!viewFrustum->isSphereInFrustum(boundSphere->getTransformedCenter(), boundSphere->getTransformedRadius()))
        {
            return;
        }
    }

    shader->setMat4("localTransform", localTransform);
    shader->setMat4("model", getPhysicsObjectTransform());

    if (skeleton)
    {
        skeleton->update(shader);
    }

    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i]->draw(shader);
    }
}

/********* DEBUG **********/

void GameObject::createDebugSphere(int depth)
{
    debugSphere = new DebugSphere();

    if (boundSphere)
    {
        debugSphere->construct(boundSphere->getCenter(), boundSphere->getRadius(), depth);
    }
}

void GameObject::renderDebugSphere(Shader *shader)
{
    if (debugSphere && viewFrustum)
    {
        mat4 transform = viewFrustum->getProjection() * viewFrustum->getView() * getPhysicsObjectTransform() * localTransform;
        debugSphere->applyTransform(transform);

        debugSphere->render(shader);
    }
}

/********* DEBUG **********/

GameObject::~GameObject()
{
    globalNames.erase(name);

    removeGraphicsObject();
    removePhysicsObject();
    
    delete modelLoader;

    delete debugSphere;

    for (auto& i: animations)
    {
        delete i.second;
    }
}
