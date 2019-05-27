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
    levelDepthBuffer = new DepthBuffer();

    objectShader = new Shader();
    dirShadowShader = new Shader();
    skyBoxShader = new Shader();

    dirLight = nullptr;
    skyBox = nullptr;

    camera = nullptr;

    /* AAAA CHANGE */
    projection = new mat4(perspective(45.0f, 1.77778f, 0.1f, 1000.0f));
    shadowProjection = new mat4(ortho(-50.0f, 50.0f, -50.0f, 50.0f, -30.0f, 30.0f));
}

void Level::loadLevel(string level)
{
    levelName = level;

    levelColorBuffer->genBuffer(window->getRenderSize());
    //levelDepthBuffer->genBuffer(2048, 2048);
    
    //objectShader->loadShaders(path("code/shader/vertexObjectShader.glsl"), path("code/shader/fragmentObjectShader.glsl"));
    //dirShadowShader->loadShaders(path("code/shader/vertexDirShadowShader.glsl"), path("code/shader/fragmentDirShadowShader.glsl"));
    skyBoxShader->loadShaders(path("code/shader/vertexSkyBoxShader.glsl"), path("code/shader/fragmentSkyBoxShader.glsl"));
    
    levelLoader->loadLevel(path("levels/test1"));

    /*** GET LOADED DATA **/
    levelLoader->getSkyBoxData(skyBox);
    levelLoader->getPlayerData(camera);
}

void Level::render()
{
    /***********************************/
    /*********** GAME RENDER ***********/
    /***********************************/

    levelColorBuffer->use();
    levelColorBuffer->clear();

    view = camera->getView();

    /************************************
    * SKYBOX 
    * */
    skyBoxShader->use();

    glUniformMatrix4fv(glGetUniformLocation(skyBoxShader->getID(), "view"), 1, GL_FALSE, value_ptr(mat4(mat3(view))));
    glUniformMatrix4fv(glGetUniformLocation(skyBoxShader->getID(), "projection"), 1, GL_FALSE, value_ptr(*projection));

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
    delete levelDepthBuffer;

    delete objectShader;
    delete dirShadowShader;
    delete skyBoxShader;

    delete dirLight;
    delete skyBox;

    delete projection;
    delete shadowProjection;
}
