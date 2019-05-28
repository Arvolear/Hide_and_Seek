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
#include "level.hpp"

Level::Level(Window* window)
{
    this->window = window;

    levelLoader = new LevelLoader(window);

    levelName = "";

    levelColorBuffer = new ColorBuffer();

    gameObjectShader = new Shader();
    dirShadowShader = new Shader();
    skyBoxShader = new Shader();

    skyBox = nullptr;

    camera = nullptr;

    /* AAAA CHANGE */
    projection = mat4(perspective(45.0f, 1.77778f, 0.1f, 100.0f));
}

void Level::loadLevel(string level)
{
    levelName = level;

    levelColorBuffer->genBuffer(window->getRenderSize());
    
    gameObjectShader->loadShaders(path("code/shader/vertexObjectShader.glsl"), path("code/shader/fragmentObjectShader.glsl"));
    dirShadowShader->loadShaders(path("code/shader/vertexDirShadowShader.glsl"), path("code/shader/fragmentDirShadowShader.glsl"));
    skyBoxShader->loadShaders(path("code/shader/vertexSkyBoxShader.glsl"), path("code/shader/fragmentSkyBoxShader.glsl"));
    

    levelLoader->loadLevel(path("levels/test1"));

    /*** GET LOADED DATA **/
    levelLoader->getDirLightData(dirLights);
    levelLoader->getSkyBoxData(skyBox);
    levelLoader->getPlayerData(camera);
    levelLoader->getGameObjectsData(gameObjects);
}

void Level::render()
{
    /***********************************/
    /*********** GAME RENDER ***********/
    /***********************************/

    mat4 view = camera->getView();

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        /************************************
         * DIR SHADOWS
         * */ 

        dirLights[i]->updateView(camera->getPosition());

        /*** depth buffer ***/
        dirLights[i]->getShadowBuffer()->use();
        dirLights[i]->getShadowBuffer()->clear();

        dirShadowShader->use();

        glUniformMatrix4fv(glGetUniformLocation(dirShadowShader->getID(), "view"), 1, GL_FALSE, value_ptr(dirLights[i]->getView()));
        glUniformMatrix4fv(glGetUniformLocation(dirShadowShader->getID(), "projection"), 1, GL_FALSE, value_ptr(dirLights[i]->getProjection()));

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

    glUniformMatrix4fv(glGetUniformLocation(gameObjectShader->getID(), "view"), 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(gameObjectShader->getID(), "projection"), 1, GL_FALSE, value_ptr(projection));

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        glUniformMatrix4fv(glGetUniformLocation(gameObjectShader->getID(), ("dirLightsMatrices[" + to_string(i) + "].shadowView").c_str()), 1, GL_FALSE, value_ptr(dirLights[i]->getView()));
        glUniformMatrix4fv(glGetUniformLocation(gameObjectShader->getID(), ("dirLightsMatrices[" + to_string(i) + "].shadowProjection").c_str()), 1, GL_FALSE, value_ptr(dirLights[i]->getProjection()));

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

    glUniformMatrix4fv(glGetUniformLocation(skyBoxShader->getID(), "view"), 1, GL_FALSE, value_ptr(mat4(mat3(view))));
    glUniformMatrix4fv(glGetUniformLocation(skyBoxShader->getID(), "projection"), 1, GL_FALSE, value_ptr(projection));

    skyBox->render(skyBoxShader);
} 

GLuint Level::getRenderTexture() const
{
    return levelColorBuffer->getTexture();
}

Camera* Level::getPlayer() const
{
    return camera;
}

Level::~Level()
{
    delete levelLoader;

    delete levelColorBuffer;

    delete gameObjectShader;
    delete dirShadowShader;
    delete skyBoxShader;

    for (auto& i : gameObjects)
    {
        delete i.second;
    }

    for (size_t i = 0; i < dirLights.size(); i++)
    {
        delete dirLights[i];
    }

    delete skyBox;
}
