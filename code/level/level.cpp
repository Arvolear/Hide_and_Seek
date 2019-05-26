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

#include "dirlight.hpp"
#include "skybox.hpp"
#include "level.hpp"

Level::Level(Window* window)
{
    this->window = window;

    levelColorBuffer = new ColorBuffer();
    levelDepthBuffer = new DepthBuffer();

    objectShader = new Shader();
    dirShadowShader = new Shader();

    dirLight = new DirLight();
    skyBox = new SkyBox();

    levelName = "";
}

void Level::loadLevel(string level)
{
    levelName = level;
    levelColorBuffer->genBuffer(window->getRenderSize());
    levelDepthBuffer->genBuffer(2048, 2048);
}

void Level::render()
{
    
}

GLuint Level::getRenderTexture() const
{
    return levelColorBuffer->getTexture();
}

Level::~Level()
{
    delete levelColorBuffer;
    delete levelDepthBuffer;

    delete objectShader;
    delete dirShadowShader;

    delete dirLight;
    delete skyBox;
}
