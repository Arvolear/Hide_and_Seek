#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/shadowbuffer.hpp"
#include "../framebuffer/gbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../global/gaussianblur.hpp"
#include "../global/radialblur.hpp"
#include "../global/poissondisk.hpp"

#include "../player/camera.hpp"

#include "../debug/debugdrawer.hpp"

#include "../world/raytracer.hpp"
#include "../world/constrainthandler.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../game_object/sphere.hpp"
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
#include "../game_object/instancedgameobject.hpp"
#include "../game_object/weapon.hpp"
#include "../game_object/rifle.hpp"

#include "../player/player.hpp"

#include "dirlightsoftshadow.hpp"
#include "dirlight.hpp"
#include "ssao.hpp"
#include "atmosphere.hpp"
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
    dirSphereShader = new Shader();
    lightBlenderShader = new Shader();
    atmosphereShader = new Shader();
    domeShader = new Shader();
    sSAOShader = new Shader();

    debugShader = new Shader();
    
    sSAO = nullptr;
    atmosphere = nullptr;
    skyBox = nullptr;

    playerID = -1;

    projection = mat4(1.0);
    viewFrustum = nullptr;

    quad = new RenderQuad();

    /* DEBUG */
    drawDebug = 0;
    activeVirtualPlayer = 0;
}

void Level::loadLevel(string level)
{
    levelName = level;

    levelColorBuffer->genBuffer(window->getRenderSize(), 
            {
                {GL_RGBA16F, GL_RGBA, GL_FLOAT}, 
                {GL_RGBA16F, GL_RGBA, GL_FLOAT}
            });

    gBuffer->genBuffer(window->getRenderSize(), 
            {
                {GL_RGB16F, GL_RGB, GL_FLOAT}, 
                {GL_RGB16F, GL_RGB, GL_FLOAT}, 
                {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE}, 
                {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE}, 
                {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE},
                {GL_R16F, GL_RED, GL_FLOAT}, 
                {GL_RGB16F, GL_RGB, GL_FLOAT}
            });
    
    gBufferShader->loadShaders(global.path("code/shader/gBufferShader.vert"), global.path("code/shader/gBufferShader.frag"));
    gameObjectShader->loadShaders(global.path("code/shader/objectShader.vert"), global.path("code/shader/objectShader.frag"));
    dirShadowShader->loadShaders(global.path("code/shader/dirShadowShader.vert"), global.path("code/shader/dirShadowShader.frag"));
    skyBoxShader->loadShaders(global.path("code/shader/skyBoxShader.vert"), global.path("code/shader/skyBoxShader.frag"));
    dirSphereShader->loadShaders(global.path("code/shader/dirSphereShader.vert"), global.path("code/shader/dirSphereShader.frag"));
    lightBlenderShader->loadShaders(global.path("code/shader/lightBlenderShader.vert"), global.path("code/shader/lightBlenderShader.frag"));
    atmosphereShader->loadShaders(global.path("code/shader/atmosphereShader.vert"), global.path("code/shader/atmosphereShader.frag"));
    domeShader->loadShaders(global.path("code/shader/domeShader.vert"), global.path("code/shader/domeShader.frag"));
    sSAOShader->loadShaders(global.path("code/shader/ssaoShader.vert"), global.path("code/shader/ssaoShader.frag"));
    
    debugShader->loadShaders(global.path("code/shader/debugShader.vert"), global.path("code/shader/debugShader.frag"));

    cout << "Loading " << levelName << " level..." << endl;

    levelLoader->loadLevel(global.path("levels/" + levelName));

    /*** GET LOADED DATA ***/
    levelLoader->getSsaoData(sSAO);
    levelLoader->getAtmosphereData(atmosphere);
    levelLoader->getDirLightData(dirLights);
    levelLoader->getSkyBoxData(skyBox);
    levelLoader->getPlayersData(players);
    levelLoader->getGameObjectsData(gameObjects);
    levelLoader->getProjectionData(projection);
    levelLoader->getViewFrustumData(viewFrustum);

    quad->init();
    
    /* DEBUG */
    levelLoader->getVirtualPlayerData(virtualPlayer);
}
        
void Level::setPlayerID(int playerID)
{
    this->playerID = playerID;
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

map < string, GameObject* > Level::getGameObjects() const
{
    return gameObjects;
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

    mat4 view = getPlayer(true)->getView();
    mat4 staticView = mat4(mat3(view));
    
    viewFrustum->updateFrustum(view, projection);
    
    /************************************
     * ATMOSPHERE
     * */
    
    glCullFace(GL_FRONT);
    
    atmosphere->getBuffer()->use();
    
    atmosphereShader->use();

    atmosphereShader->setMat4("view", staticView);
    atmosphereShader->setMat4("projection", projection);

    atmosphere->renderAtmosphere(atmosphereShader);

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
            /* crucial */
            dirLights[i]->getShadowBuffer()->clear(vec4(1.0, 0.0, 0.0, 1.0));

            dirLights[i]->updateShadowView(getPlayer(true)->getPosition());

            dirShadowShader->use();

            dirShadowShader->setMat4("view", dirLights[i]->getShadowView());
            dirShadowShader->setMat4("projection", dirLights[i]->getShadowProjection());

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
     * SSAO
     * */ 

    sSAO->getBuffer()->use();
    sSAO->getBuffer()->clear();

    sSAOShader->use();
    
    sSAOShader->setMat4("invProjection", transpose(inverse(projection)));
    sSAOShader->setMat4("projection", projection);

    gBuffer->renderSsao(sSAOShader);
    sSAO->renderInfo(sSAOShader);

    quad->render(sSAOShader);

    /************************************
     * GAMEOBJECT
     * */ 

    glCullFace(GL_BACK);

    /*** color buffer ***/
    levelColorBuffer->use();

    glDisable(GL_DEPTH_TEST);

    gameObjectShader->use();

    gameObjectShader->setVec3("viewPos", getPlayer(true)->getPosition());

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        dirLights[i]->blurShadow(1, 1.0);

        levelColorBuffer->use();
        gameObjectShader->use();

        dirLights[i]->renderShadow(gameObjectShader, i);
    }

    sSAO->blur(1, 1.0);
        
    levelColorBuffer->use();
    gameObjectShader->use();

    sSAO->renderSsao(gameObjectShader);

    gBuffer->render(gameObjectShader);
    quad->render(gameObjectShader);

    glEnable(GL_DEPTH_TEST);

    /************************************
     * LIGHT SCATTERER
     * */

    glCullFace(GL_BACK);

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        if (dirLights[i]->getScatterBuffer())
        {
            /*** scatter buffer ***/
            dirLights[i]->getScatterBuffer()->copyDepthBuffer(gBuffer);
            /* crusial */
            dirLights[i]->getScatterBuffer()->copyColorBuffer(0, gBuffer, 4);
            dirLights[i]->getScatterBuffer()->use();

            dirSphereShader->use();

            dirSphereShader->setMat4("view", staticView);
            dirSphereShader->setMat4("projection", projection);

            dirLights[i]->renderSphere(dirSphereShader);

            /* radial blur center */
            vec4 center = projection * staticView * vec4(dirLights[i]->getSphere()->getCenter(), 1.0);

            center /= center.w;

            dirLights[i]->blurScatter(vec2(center));
        }
    }

    levelColorBuffer->copyDepthBuffer(gBuffer);
    levelColorBuffer->use();

    /************************************
     * DOME (atmosphere)
     * */

    glCullFace(GL_BACK);

    domeShader->use();

    atmosphere->renderDome(domeShader);

    /************************************
     * SKYBOX 
     * */

    glCullFace(GL_BACK);

    skyBoxShader->use();

    skyBoxShader->setMat4("view", staticView);
    skyBoxShader->setMat4("projection", projection);

    //skyBox->render(skyBoxShader);

    /************************************
     * SCATTERED LIGHT BLENDING
     * */

    glCullFace(GL_BACK);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    lightBlenderShader->use();

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        dirLights[i]->renderLight(lightBlenderShader);
        quad->render(lightBlenderShader);
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

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

    if (activeVirtualPlayer)
    {
        virtualPlayer->update(!mode);
    }
}

void Level::updateSunPos()
{
    atmosphere->updateSunPos();
    vec3 sunPos = atmosphere->getSunPos();

    dirLights[0]->setDirection(-normalize(sunPos));
    dirLights[0]->getSphere()->setCenter(sunPos);
}

GLuint Level::getRenderTexture(unsigned int num) const
{
    return levelColorBuffer->getTexture(num);
    //return atmosphere->getTexture();
    //return sSAO->getTexture();
}

Player* Level::getPlayer(bool andVirtual) const
{
    if (activeVirtualPlayer && andVirtual)
    {
        return virtualPlayer;    
    }

    for (size_t i = 0; i < players.size(); i++)
    {
        if (players[i]->getID() == playerID)
        {
            return players[i];
        }
    }

    return nullptr;
}

vector < Player* > Level::getPlayers() const
{
    return players;
}

string Level::getLevelName() const
{
    return levelName;
}

/* DEBUG */
void Level::toggleDebug()
{
    drawDebug = (drawDebug + 1) % 2;
}
        
void Level::toggleVirtualPlayer()
{
    activeVirtualPlayer = (activeVirtualPlayer + 1) % 2;

    getPlayer()->setActive(!activeVirtualPlayer);
    getPlayer()->resetPrevCoords();

    virtualPlayer->setActive(activeVirtualPlayer);
    virtualPlayer->resetPrevCoords();
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
    delete dirSphereShader;
    delete lightBlenderShader;
    delete atmosphereShader;
    delete domeShader;
    delete sSAOShader;

    delete debugShader;

    for (auto& i : gameObjects)
    {
        delete i.second;
    }
    
    for (size_t i = 0; i < dirLights.size(); i++)
    {
        delete dirLights[i];
    }

    delete sSAO;
    delete atmosphere;
    delete skyBox;

    for (size_t i = 0; i < players.size(); i++)
    {
        delete players[i];
    }

    delete virtualPlayer;

    delete viewFrustum;
    delete quad;
}
