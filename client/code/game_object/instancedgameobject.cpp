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

void InstancedGameObject::addRadius(float radius)
{
    radiuses.push_back(radius);
}

void InstancedGameObject::addBorders(vec2 leftTop, vec2 rightBottom)
{
    leftTops.push_back(leftTop);
    rightBottoms.push_back(rightBottom);
}

void InstancedGameObject::genInstances()
{
    if (radiuses.size() != leftTops.size())
    {
        throw(runtime_error("ERROR::InstancedGameObject::genInstances() radiuses != borders"));
    }

    for (size_t k = 0; k < radiuses.size(); k++)
    {
        float radius = radiuses[k];
        vec2 leftTop = leftTops[k];
        vec2 rightBottom = rightBottoms[k];

        poissonDisk->setRadius(radius);
        poissonDisk->setSize(abs(rightBottom.x - leftTop.x), abs(leftTop.y - rightBottom.y));
        poissonDisk->generate();

        vector < PoissonDisk::GridInfo > disk = poissonDisk->getDisk();

        for (size_t i = 0; i < disk.size(); i++)
        {
            disk[i].x += leftTop.x;
            disk[i].y += leftTop.y;

            mat4 trans = mat4(1.0);

            trans *= translate(vec3(disk[i].x, 0.0, disk[i].y));
            trans *= rotate(global.getRandomNumber() * 2 - 1, vec3(0.0, 1.0, 0.0));

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
