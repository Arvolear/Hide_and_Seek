#include "../global/globaluse.hpp"
#include "../global/poissondisk.hpp"

#include "../shader/shader.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "../debug/debugdrawer.hpp"

#include "../world/raytracer.hpp"
#include "../world/constrainthandler.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "sphere.hpp"
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
#include "instancedgameobject.hpp"

InstancedGameObject::InstancedGameObject(string name ) : GameObject(name) 
{
    poissonDisk = new PoissonDisk();
}

void InstancedGameObject::setRadius(int index, float radius)
{
    radiuses.insert({index, radius});
}

void InstancedGameObject::setBorders(int index, vec2 leftTop, vec2 rightBottom)
{
    leftTops.insert({index, leftTop});
    rightBottoms.insert({index, rightBottom});
}
        
void InstancedGameObject::setWithoutPolygons(int index, vector < vector < vec2 > > without)
{
    withouts.insert({index, without});
}

void InstancedGameObject::genInstances()
{
    for (auto& k: radiuses)
    {
        if (leftTops.find(k.first) == leftTops.end())
        {
            throw(runtime_error("ERROR::InstancedGameObject::genInstances() radiuses != borders"));
        }

        float radius = k.second;
        vec2 leftTop = leftTops.find(k.first)->second;
        vec2 rightBottom = rightBottoms.find(k.first)->second;

        auto without = withouts.find(k.first);
        if (without != withouts.end())
        {
            poissonDisk->setWithoutPolygons(without->second);
        }

        poissonDisk->setRadius(radius);
        poissonDisk->setBorders(leftTop, rightBottom);

        poissonDisk->generate();

        vector < vec2 > disk = poissonDisk->getDisk();

        for (size_t i = 0; i < disk.size(); i++)
        {
            mat4 trans = mat4(1.0);

            trans *= translate(vec3(disk[i].x, 0.0, disk[i].y));
            trans *= rotate(float(global.getRandomNumber() * 2 * 3.14159265), vec3(0.0, 1.0, 0.0));
            trans *= scale(vec3(global.getRandomNumber() + 0.5));

            transformations.push_back(trans);
        }
    }

    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i]->setupInstancedMesh(transformations); 
    }
}

void InstancedGameObject::render(Shader* shader, bool viewCull)
{
    if (skeleton)
    {
        skeleton->update(shader);
    }

    if (visible)
    {
        if (!cull)
        {
            glDisable(GL_CULL_FACE);
        }

        unique_lock < mutex > lk(mtx);
        ready = false;

        shader->setMat4("localTransform", localTransform);        
        shader->setMat4("model", getPhysicsObjectTransform());

        ready = true;
        lk.unlock();
        cv.notify_all();

        for (size_t i = 0; i < meshes.size(); i++)
        {
            meshes[i]->render(shader, true);
        }
        
        glEnable(GL_CULL_FACE);
    }
}

InstancedGameObject::~InstancedGameObject() 
{
    delete poissonDisk;
}
