#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../debug/debugsphere.hpp"
#include "../debug/debugdrawer.hpp"

#include "../player/camera.hpp"

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

#include "../player/player.hpp"

#include "dirlight.hpp"
#include "skybox.hpp"
#include "levelloader.hpp"

LevelLoader::LevelLoader(Window* window, World* physicsWorld)
{
    this->window = window;
    this->physicsWorld = physicsWorld;

    skyBox = nullptr;
    player = nullptr;

    projection = mat4(1.0);
}

void LevelLoader::loadObjects()
{
    /* TODO PARSE XML */
    GameObject* GO0 = new GameObject("floor"); // floor
    GO0->setGraphicsObject(levelName + "/static_models/Floor/floor.obj");
    GO0->setPhysicsObject(new PhysicsObject(physicsWorld->getWorld(), new btCylinderShape(btVector3(76, 0.93, 76)), 0, btVector3(0.0f, 0.0f, 0.0f)));
    GO0->createDebugSphere(3);
    
    gameObjects.insert({GO0->getName(), GO0});

    GO0 = new GameObject("box");
    GO0->setGraphicsObject(levelName + "/static_models/Box/box.obj");
    GO0->setPhysicsObject(new PhysicsObject(physicsWorld->getWorld()));

    CompoundShape* CS = new CompoundShape;

    CS->add(new btBoxShape(btVector3(1.46, 1.46, 1.46)), btVector3(0, 0.7, 0));
    CS->add(new btBoxShape(btVector3(1.46, 1.46, 1.46)), btVector3(0.3, -0.7, 0));
    CS->add(new btBoxShape(btVector3(1.46, 1.46, 1.46)), btVector3(-0.7, -0.3, 0));
    CS->add(new btBoxShape(btVector3(1.46, 1.46, 1.46)), btVector3(0.7, 0.7, 0));

    GO0->getPhysicsObject()->setShape(CS);
    GO0->getPhysicsObject()->setShape(new btBoxShape(btVector3(1.46, 1.46, 1.46)));
    GO0->getPhysicsObject()->setPosition(btVector3(-0.8f, 30.0f, 0.0f));
    GO0->getPhysicsObject()->setMass(100);
    GO0->getPhysicsObject()->setRotation(btQuaternion(btVector3(0, 0, 1), toRads(60)));

    /* game object constructor verifies the name uniqueness */
    gameObjects.insert({GO0->getName(), GO0});


    GameObject* GO1 = new GameObject("Buddha");
    GO1->setLocalScale(vec3(0.2, 0.2, 0.2));
    GO1->setGraphicsObject(levelName + "/static_models/Buddha/Buddha.obj");

    gameObjects.insert({GO1->getName(), GO1});
}

void LevelLoader::loadDirLight()
{
    /* TODO PARSE XML */
    DirLight* DL0 = new DirLight();
    
    DL0->setDirection(vec3(-1.0f, -1.0f, -0.2f));
    DL0->setAmbient(vec3(0.2f, 0.2f, 0.2f));
    DL0->setDiffuse(vec3(0.8f, 0.8f, 0.8f));
    DL0->setSpecular(vec3(0.5f, 0.5f, 0.5f));

    DL0->genShadowBuffer(2048, 2048);
    DL0->setProjection(ortho(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f));

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
    GameObject* P = new GameObject("player");
    P->setPhysicsObject(new PhysicsObject(physicsWorld->getWorld()));

    CompoundShape* CS = new CompoundShape;

    CS->add(new btSphereShape(0.6), btVector3(0, 3, 0), btQuaternion(btVector3(0, 0, 1), 0));
    CS->add(new btCapsuleShape(0.4, 3.5), btVector3(0, 0, 0), btQuaternion(btVector3(0, 0, 1), 0));

    P->getPhysicsObject()->setShape(CS);
    P->getPhysicsObject()->setPosition(btVector3(10, 3, 10));
    P->getPhysicsObject()->setMass(10);
    P->getPhysicsObject()->getRigidBody()->setAngularFactor(btVector3(0, 0, 0));

    RayTracer* rayTracer = new RayTracer(physicsWorld->getWorld(), nullptr, projection);     

    player = new Player(window, vec3(0, 5, 5), vec3(0, 0, -1), rayTracer, P);
    rayTracer->setCamera(player);
}

void LevelLoader::loadProjection()
{
    projection = mat4(perspective(45.0f, 1.77778f, 0.1f, 500.0f));
}

void LevelLoader::loadLevel(string name)
{
    this->levelName = name;

    loadObjects();
    loadDirLight();
    loadSkyBox();
    
    loadProjection();

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
 
void LevelLoader::getProjectionData(mat4 &projection) const
{
    projection = this->projection;
}

void LevelLoader::getPlayerData(Player*& player) const
{
    player = this->player;
}

LevelLoader::~LevelLoader() {}
