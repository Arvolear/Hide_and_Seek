#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "../debug/debugsphere.hpp"
#include "../debug/debugdrawer.hpp"

#include "../world/raytracer.hpp"
#include "../world/constrainthandler.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../game_object/openglmotionstate.hpp"
#include "../game_object/animation.hpp"
#include "../game_object/mesh.hpp"
#include "../game_object/bone.hpp"
#include "../game_object/skeleton.hpp"
#include "../game_object/viewfrustum.hpp"
#include "../game_object/boundsphere.hpp"
#include "../game_object/modelloader.hpp"
#include "../game_object/physicsobject.hpp"
#include "../game_object/gameobject.hpp"

#include "dirlight.hpp"
#include "skybox.hpp"
#include "levelloader.hpp"

LevelLoader::LevelLoader(Window* window, World* physicsWorld)
{
    this->window = window;
    this->physicsWorld = physicsWorld;

    skyBox = nullptr;
    camera = nullptr;
}

void LevelLoader::loadObjects()
{
    /* TODO PARSE XML */
    GameObject* GO0 = new GameObject("floor"); //floor
    GO0->setGraphicsObject(levelName + "/static_models/Floor/floor.obj");
    GO0->setWorld(physicsWorld->getWorld());
    GO0->setPhysicsObject(new PhysicsObject(new btCylinderShape(btVector3(76, 0.93, 76)), 0, btVector3(0.0f, 0.0f, 0.0f), btQuaternion(btVector3(0, 0, 1), 0)));
    GO0->createDebugSphere(3);
    
    gameObjects.insert({GO0->getName(), GO0});

    GO0 = new GameObject("box");
    GO0->setGraphicsObject(levelName + "/static_models/Box/box.obj");
    GO0->setWorld(physicsWorld->getWorld());
    GO0->setPhysicsObject(new PhysicsObject(new btBoxShape(btVector3(1.46, 1.46, 1.46)), 100.0, btVector3(-0.8f, 30.0f, 0.0f), btQuaternion(btVector3(0, 0, 1), 0)));

    /* game object constructor verifies the name uniqueness */
    gameObjects.insert({GO0->getName(), GO0});
}

void LevelLoader::loadDirLight()
{
    /* TODO PARSE XML */
    DirLight* DL0 = new DirLight();
    
    DL0->setDirection(vec3(-1.0f, -1.0f, -0.2f));
    DL0->setAmbient(vec3(0.2f, 0.1f, 0.1f));
    DL0->setDiffuse(vec3(1.0f, 0.2f, 0.2f));
    DL0->setSpecular(vec3(0.8f, 0.2f, 0.2f));

    DL0->genShadowBuffer(2048, 2048);
    DL0->setProjection(ortho(-50.0f, 50.0f, -50.0f, 50.0f, -30.0f, 30.0f));

    if (find(dirLights.begin(), dirLights.end(), DL0) == dirLights.end())
    {
        dirLights.push_back(DL0); 
    }
}

void LevelLoader::loadSkyBox()
{
    /* TODO PARSE XML */
    skyBox = new SkyBox();
    skyBox->init();

    //skyBox->loadSkyBox(levelName + "/skyboxes/Bluerocky");
    skyBox->loadSkyBox(levelName + "/skyboxes/Space");
}

void LevelLoader::loadPlayer()
{
    /* TODO PARSE XML */
    camera = new Camera(window, vec3(0, 5, 5), vec3(0, 0, -1), 0.5);
}

void LevelLoader::loadLevel(string name)
{
    this->levelName = name;

    loadObjects();
    loadDirLight();
    loadSkyBox();

    loadPlayer();
}

void LevelLoader::getGameObjectsData(map < string, GameObject* > &gameObjects) const
{
    gameObjects = this->gameObjects;
}

void LevelLoader::getDirLightData(vector < DirLight* > &dirLights) const
{
    dirLights = this->dirLights;
}

void LevelLoader::getSkyBoxData(SkyBox*& skyBox) const
{
    skyBox = this->skyBox;
}

void LevelLoader::getPlayerData(Camera*& player) const
{
    player = this->camera;
}

LevelLoader::~LevelLoader() {}
