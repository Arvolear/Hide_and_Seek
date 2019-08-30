#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../debug/debugsphere.hpp"

#include "animation.hpp"
#include "mesh.hpp"
#include "bone.hpp"
#include "skeleton.hpp"
#include "viewfrustum.hpp"
#include "boundsphere.hpp"
#include "modelloader.hpp"
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
    setVisible(true);
    setShadow(true);

    modelLoader = new ModelLoader();

    skeleton = nullptr;
    viewFrustum = nullptr;
    boundSphere = nullptr;
    debugSphere = nullptr;

    localTransform = modelTransform = mat4(1.0);
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
        
void GameObject::setVisible(bool visible)
{
    this->visible = visible;
}

void GameObject::setShadow(bool shadow)
{
    this->shadow = shadow;
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

void GameObject::setLocalRotation(vec3 axis, float angle, bool add)
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
    localTransform *= rotate(radians(angle), axis);

    if (add)
    {
        localTransform *= toMat4(conjugate(rot));
    }
}

void GameObject::setLocalScale(vec3 growth, bool add)
{
    vec3 sc;
    quat rot;
    vec3 tran;
    vec3 skew;
    vec4 perspective;

    decompose(localTransform, sc, rot, tran, skew, perspective);

    localTransform = mat4(1.0);
    localTransform *= translate(tran);
    localTransform *= scale(growth);

    if (add)
    {
        localTransform *= scale(sc);
    }

    localTransform *= toMat4(conjugate(rot));
}

void GameObject::setLocalPosition(vec3 translation, bool add)
{
    vec3 sc;
    quat rot;
    vec3 tran;
    vec3 skew;
    vec4 perspective;

    decompose(localTransform, sc, rot, tran, skew, perspective);

    localTransform = mat4(1.0);
    localTransform *= translate(translation);

    if (add)
    {
        localTransform *= translate(tran);
    }

    localTransform *= scale(sc);
    localTransform *= toMat4(conjugate(rot));
}
        
void GameObject::setLocalTransform(mat4 localTransform)
{
    this->localTransform = localTransform;
}

void GameObject::setModelTransform(mat4 modelTransform)
{
    this->modelTransform = modelTransform;
}

void GameObject::clearLocalTransform()
{
    localTransform = mat4(1.0);
}
        
void GameObject::addAnimation(Animation* anim)
{
    if (animations.find(anim->getName()) == animations.end())
    {
        animations.insert({anim->getName(), anim});
    }
}
        
void GameObject::removeAnimation(string name)
{
    if (animations.find(name) != animations.end())
    {
        Animation* anim = animations.find(name)->second;

        animations.erase(name);

        delete anim;
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

string GameObject::getGraphicsObject() const
{
    return graphicsObject;
}
        
Skeleton* GameObject::getSkeleton() const
{
    return skeleton;
}

string GameObject::getName() const
{
    return name;
}

bool GameObject::isVisible() const
{
    return visible;
}

bool GameObject::isShadow() const
{
    return shadow;
}

mat4 GameObject::getLocalTransform() const
{
    return localTransform;
}

mat4 GameObject::getModelTransform() const
{
    return modelTransform;
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
    if (visible && cull && viewFrustum && boundSphere)
    {
        mat4 transform = getModelTransform() * getLocalTransform();

        boundSphere->applyTransform(transform);

        if (!viewFrustum->isSphereInFrustum(boundSphere->getTransformedCenter(), boundSphere->getTransformedRadius()))
        {
            return;
        }
    }

    shader->setMat4("localTransform", getLocalTransform());
    shader->setMat4("model", getModelTransform());

    if (skeleton)
    {
        skeleton->update(shader);
    }

    if (visible)
    {
        for (size_t i = 0; i < meshes.size(); i++)
        {
            meshes[i]->render(shader);
        }
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
        mat4 transform = viewFrustum->getProjection() * viewFrustum->getView() * getModelTransform() * getLocalTransform();
        debugSphere->applyTransform(transform);

        debugSphere->render(shader);
    }
}

/********* DEBUG **********/

GameObject::~GameObject()
{
    globalNames.erase(name);

    removeGraphicsObject();

    delete modelLoader;

    delete debugSphere;

    for (auto& i: animations)
    {
        delete i.second;
    }
}
