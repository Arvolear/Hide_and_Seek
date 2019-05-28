#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../debug/debugsphere.hpp"

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

#include "../player/camera.hpp"

#include "dirlight.hpp"
#include "skybox.hpp"
#include "levelloader.hpp"

LevelLoader::LevelLoader(Window* window)
{
    this->window = window;

    skyBox = nullptr;
    camera = nullptr;
}

void LevelLoader::loadObjects()
{
    /* PARSE XML */
    GameObject* GO0 = new GameObject();
    GO0->setGraphicsObject(levelName + "/static_models/Box/box.obj");
    int index = GO0->getIndex();

    if (gameObjects.find(index) == gameObjects.end())
    {
        gameObjects.insert({index, GO0});
    }
}

void LevelLoader::loadDirLight()
{
    /* PARSE XML */
    DirLight* DL0 = new DirLight();
    
    DL0->setDirection(vec3(0.16f, -1.0f, 0.3f));
    DL0->setAmbient(vec3(0.4f, 0.4f, 0.4f));
    DL0->setDiffuse(vec3(1.0f, 1.0f, 1.0f));
    DL0->setSpecular(vec3(0.5f, 0.5f, 0.5f));

    DL0->genShadowBuffer(2048, 2048);
    DL0->setProjection(ortho(-50.0f, 50.0f, -50.0f, 50.0f, -30.0f, 30.0f));

    if (find(dirLights.begin(), dirLights.end(), DL0) == dirLights.end())
    {
        dirLights.push_back(DL0); 
    }
}

void LevelLoader::loadSkyBox()
{
    skyBox = new SkyBox();
    skyBox->init();

    skyBox->loadSkyBox(levelName + "/skyboxes/Bluerocky");
}

void LevelLoader::loadPlayer()
{
    camera = new Camera(window, vec3(0, 5, 0), vec3(0, 0, -1), 1);
}

void LevelLoader::loadLevel(string name)
{
    this->levelName = name;

    loadObjects();
    loadDirLight();
    loadSkyBox();

    loadPlayer();
}

void LevelLoader::getGameObjectsData(map < unsigned long int, GameObject* > &gameObjects) const
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
