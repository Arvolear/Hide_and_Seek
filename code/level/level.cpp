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

#include "../player/player.hpp"

#include "dirlight.hpp"
#include "skybox.hpp"
#include "levelloader.hpp"
#include "level.hpp"

Level::Level(Window* window, World* physicsWorld)
{
    this->window = window;
    this->physicsWorld = physicsWorld;

    levelLoader = new LevelLoader(window, physicsWorld);

    levelName = "";

    levelColorBuffer = new ColorBuffer();

    gameObjectShader = new Shader();
    dirShadowShader = new Shader();
    skyBoxShader = new Shader();

    debugShader = new Shader();

    skyBox = nullptr;

    projection = mat4(1.0);
    viewFrustum = nullptr;

    drawDebug = 0;
}

void Level::loadLevel(string level)
{
    levelName = level;

    levelColorBuffer->genBuffer(window->getRenderSize());
    
    gameObjectShader->loadShaders(path("code/shader/vertexObjectShader.glsl"), path("code/shader/fragmentObjectShader.glsl"));
    dirShadowShader->loadShaders(path("code/shader/vertexDirShadowShader.glsl"), path("code/shader/fragmentDirShadowShader.glsl"));
    skyBoxShader->loadShaders(path("code/shader/vertexSkyBoxShader.glsl"), path("code/shader/fragmentSkyBoxShader.glsl"));
    
    debugShader->loadShaders(path("code/shader/vertexDebugShader.glsl"), path("code/shader/fragmentDebugShader.glsl"));

    levelLoader->loadLevel(path("levels/test1"));

    /*** GET LOADED DATA ***/
    levelLoader->getDirLightData(dirLights);
    levelLoader->getSkyBoxData(skyBox);
    levelLoader->getPlayersData(players);
    levelLoader->getGameObjectsData(gameObjects);
    levelLoader->getProjectionData(projection);
    levelLoader->getViewFrustumData(viewFrustum);
}

void Level::render()
{
    /***********************************/
    /*********** GAME RENDER ***********/
    /***********************************/

    mat4 view = players[0]->getView();
    
    viewFrustum->updateFrustum(view, projection);

    /************************************
     * DIR SHADOWS
     * */ 

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        /*** depth buffer ***/
        dirLights[i]->getShadowBuffer()->use();
        dirLights[i]->getShadowBuffer()->clear();

        dirLights[i]->updateView(players[0]->getPosition());

        dirShadowShader->use();

        dirShadowShader->setMat4("view", dirLights[i]->getView());
        dirShadowShader->setMat4("projection", dirLights[i]->getProjection());

        for (auto& i : gameObjects)
        {
            i.second->render(gameObjectShader); 
        }
    }

    /************************************
     * GAMEOBJECT
     * */ 

    /*** color buffer ***/
    levelColorBuffer->use();
    levelColorBuffer->clear();

    gameObjectShader->use();

    gameObjectShader->setMat4("view", view);
    gameObjectShader->setMat4("projection", projection);
    gameObjectShader->setVec3("viewPos", players[0]->getPosition());

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        gameObjectShader->setMat4("dirLightsMatrices[" + to_string(i) + "].shadowView", dirLights[i]->getView());
        gameObjectShader->setMat4("dirLightsMatrices[" + to_string(i) + "].shadowProjection", dirLights[i]->getProjection());

        dirLights[i]->render(gameObjectShader, i);
    }

    for (auto& i : gameObjects)
    {
        i.second->render(gameObjectShader); 
    }

    /************************************
     * SKYBOX 
     * */
    skyBoxShader->use();

    skyBoxShader->setMat4("view", mat4(mat3(view)));
    skyBoxShader->setMat4("projection", projection);

    skyBox->render(skyBoxShader);
    
    /************************************
     * DEBUG
     * */
    glDisable(GL_CULL_FACE);

    debugShader->use();
    physicsWorld->getDebugDrawer()->applyViewProjection(debugShader, view, projection);
   
    physicsWorld->renderDebug();

    if (drawDebug)
    {
        for (auto& i : gameObjects)
        {
            i.second->renderDebugSphere(debugShader); 
        }
       
        for (size_t i = 0; i < players.size(); i++)
        {
            viewFrustum->updateFrustum(players[i]->getView(), projection);
            viewFrustum->render(physicsWorld->getDebugDrawer());
        }
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
} 
        
void Level::addGameObject(GameObject* gameObject)
{
    if (gameObjects.find(gameObject->getName()) == gameObjects.end())
    {
        gameObjects.insert({gameObject->getName(), gameObject}); 
    }
}

GameObject* Level::getGameObject(string name) const
{
    if (gameObjects.find(name) != gameObjects.end())
    {
        return gameObjects.find(name)->second;
    }

    return nullptr;
}

void Level::removeGameObject(GameObject* gameObject)
{
    if (gameObjects.find(gameObject->getName()) != gameObjects.end())
    {
        gameObjects.erase(gameObjects.find(gameObject->getName()));
    }
}
        
void Level::removeGameObject(string name)
{
    if (gameObjects.find(name) != gameObjects.end())
    {
        gameObjects.erase(gameObjects.find(name));
    }
}

GLuint Level::getRenderTexture() const
{
    return levelColorBuffer->getTexture();
}

Player* Level::getPlayer() const
{
    return players[0];
}

void Level::toggleDebug()
{
    drawDebug = (drawDebug + 1) % 2;
}
        
void Level::swapPlayers()
{
    rotate(players.begin(), players.begin() + 1, players.end());
    players[0]->resetPrevCoords();
}

Level::~Level()
{
    delete levelLoader;

    delete levelColorBuffer;

    delete gameObjectShader;
    delete dirShadowShader;
    delete skyBoxShader;

    delete debugShader;

    for (auto& i : gameObjects)
    {
        delete i.second;
    }

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        delete dirLights[i];
    }

    delete skyBox;

    for (size_t i = 0; i < players.size(); i++)
    {
        delete players[i];
    }

    delete viewFrustum;
}
