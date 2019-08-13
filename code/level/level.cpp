#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/depthcolorbuffer.hpp"
#include "../framebuffer/gbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../global/gaussianblur.hpp"

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
#include "../game_object/weapon.hpp"
#include "../game_object/rifle.hpp"

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
    gBuffer = new GBuffer();

    gBufferShader = new Shader();
    gameObjectShader = new Shader();
    dirShadowShader = new Shader();
    skyBoxShader = new Shader();

    debugShader = new Shader();

    skyBox = nullptr;

    projection = mat4(1.0);
    viewFrustum = nullptr;

    quad = new RenderQuad();

    drawDebug = 0;
}

void Level::loadLevel(string level)
{
    levelName = level;

    levelColorBuffer->genBuffer(window->getRenderSize(), 2);
    gBuffer->genBuffer(window->getRenderSize());
    
    gBufferShader->loadShaders(path("code/shader/gBufferShader.vert"), path("code/shader/gBufferShader.frag"));
    gameObjectShader->loadShaders(path("code/shader/objectShader.vert"), path("code/shader/objectShader.frag"));
    dirShadowShader->loadShaders(path("code/shader/dirShadowShader.vert"), path("code/shader/dirShadowShader.frag"));
    skyBoxShader->loadShaders(path("code/shader/skyBoxShader.vert"), path("code/shader/skyBoxShader.frag"));
    
    debugShader->loadShaders(path("code/shader/debugShader.vert"), path("code/shader/debugShader.frag"));

    levelLoader->loadLevel(path("levels/test1"));

    /*** GET LOADED DATA ***/
    levelLoader->getDirLightData(dirLights);
    levelLoader->getSkyBoxData(skyBox);
    levelLoader->getPlayersData(players);
    levelLoader->getGameObjectsData(gameObjects);
    levelLoader->getProjectionData(projection);
    levelLoader->getViewFrustumData(viewFrustum);
    
    quad->init();
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

void Level::render()
{
    /***********************************/
    /*********** GAME RENDER ***********/
    /***********************************/

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    mat4 view = players[0]->getView();
    
    viewFrustum->updateFrustum(view, projection);

    /************************************
     * DIR SHADOWS
     * */ 
    
    glCullFace(GL_FRONT);

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        if (dirLights[i]->getShadowBuffer())
        {
            /*** shadow buffer ***/
            dirLights[i]->getShadowBuffer()->use();
            dirLights[i]->getShadowBuffer()->clear();

            dirLights[i]->updateView(players[0]->getPosition());

            dirShadowShader->use();

            dirShadowShader->setMat4("view", dirLights[i]->getView());
            dirShadowShader->setMat4("projection", dirLights[i]->getProjection());

            for (auto& i : gameObjects)
            {
                if (i.second->isShadow())
                {
                    i.second->render(dirShadowShader); 
                }
            }
        }
    }

    /************************************
     * GBUFFER
     * */ 

    glCullFace(GL_BACK);

    /*** gbuffer ***/
    gBuffer->use();
    gBuffer->clear();

    gBufferShader->use();

    gBufferShader->setMat4("view", view);
    gBufferShader->setMat4("projection", projection);

    for (auto& i : gameObjects)
    {
        i.second->render(gBufferShader); 
    }

    /************************************
     * GAMEOBJECT
     * */ 

    /*** color buffer ***/
    levelColorBuffer->use();
    levelColorBuffer->clear();

    gameObjectShader->use();

    gameObjectShader->setVec3("viewPos", players[0]->getPosition());

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        dirLights[i]->blur(1, 1);

        levelColorBuffer->use();
        gameObjectShader->use();

        dirLights[i]->render(gameObjectShader, i);
    }

    gBuffer->render(gameObjectShader);
    quad->render(gameObjectShader);

    /************************************
     * SKYBOX 
     * */

    levelColorBuffer->copyDepthBuffer(gBuffer);
    levelColorBuffer->use();

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
    physicsWorld->getDebugDrawer()->updateViewProjection();

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
} 

void Level::updatePlayers(int mode)
{
    for (size_t i = 0; i < players.size(); i++)
    {
        players[i]->update(!mode);
    }
}

GLuint Level::getRenderTexture(unsigned int num) const
{
    return levelColorBuffer->getTexture(num);
    //return gBuffer->getTexture(3);
}

Player* Level::getPlayer() const
{
    return players[0];
}

string Level::getLevelName() const
{
    return levelName;
}

void Level::toggleDebug()
{
    drawDebug = (drawDebug + 1) % 2;
}

void Level::swapPlayers()
{
    rotate(players.begin(), players.begin() + 1, players.end());

    players[0]->setActive(true);
    players[0]->resetPrevCoords();

    players[players.size() - 1]->setActive(false);
}

Level::~Level()
{
    delete levelLoader;

    delete levelColorBuffer;
    delete gBuffer;

    delete gBufferShader;
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
    delete quad;
}
