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

    dirLight = nullptr;
    skyBox = nullptr;
    camera = nullptr;
}

void LevelLoader::loadObjects()
{
        
}

void LevelLoader::loadDirLight()
{

}

void LevelLoader::loadSkyBox()
{
    skyBox = new SkyBox();
    skyBox->init();

    skyBox->loadSkyBox(path("levels/test1/skyboxes/Bluerocky"));
}
        
void LevelLoader::loadPlayer()
{
    camera = new Camera(window, vec3(0, 5, 0), vec3(0, 0, -1));
}
        
void LevelLoader::loadLevel(string name)
{
    this->levelName = name;

    loadObjects();
    loadDirLight();
    loadSkyBox();

    loadPlayer();
}

void LevelLoader::getGameObjectsData(map < size_t, GameObject* > &gameObjects) const
{
    gameObjects = this->gameObjects;
}

void LevelLoader::getPhysicsObjectsData(map < PhysicsObject*, GameObject* > &physicsObjects) const
{
    physicsObjects = this->physicsObjects;
}

void LevelLoader::getDirLightData(DirLight*& dirLight) const
{
    dirLight = this->dirLight;
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
