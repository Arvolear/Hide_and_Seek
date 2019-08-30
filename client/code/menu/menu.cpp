#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/depthcolorbuffer.hpp"
#include "../framebuffer/gbuffer.hpp"

#include "../window/glfwevents.hpp"
#include "../window/renderquad.hpp"
#include "../window/window.hpp"

#include "../global/gaussianblur.hpp"

#include "../player/camera.hpp"

#include "../debug/debugsphere.hpp"

#include "../game_object/animation.hpp"
#include "../game_object/mesh.hpp"
#include "../game_object/bone.hpp"
#include "../game_object/skeleton.hpp"
#include "../game_object/viewfrustum.hpp"
#include "../game_object/boundsphere.hpp"
#include "../game_object/modelloader.hpp"
#include "../game_object/gameobject.hpp"

#include "../player/player.hpp"

#include "../multiplayer/client.hpp"
#include "../multiplayer/playerdatacollector.hpp"

#include "../level/dirlight.hpp"
#include "../level/skybox.hpp"
#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "../multiplayer/client.hpp"
#include "../multiplayer/playerdatacollector.hpp"
#include "../multiplayer/playerdataupdater.hpp"
#include "../multiplayer/multiplayer.hpp"

#include "../game/game.hpp"

#include "menu.hpp"

Menu::Menu()
{
    window = nullptr;

    menuShader = new Shader();
    menuBuffer = new ColorBuffer();

    game = nullptr;
}

void Menu::checkEvents()
{
    if (window->isKeyPressed(GLFW_KEY_ESCAPE))
    {
        window->close();
    }
    
    if (window->isKeyPressed(GLFW_KEY_ENTER))
    {
        game = new Game(window, "test1");
        game->gameLoop();
    }
}

void Menu::menuLoop()
{
    window = new Window();
    menuBuffer->genBuffer(window->getRenderSize());

    while (window->isOpen())
    {
        window->pollEvents();
        checkEvents();

        window->render(menuBuffer->getTexture());
    }
}

Menu::~Menu()
{
    delete menuShader;
    delete menuBuffer;

    //delete settings;
    delete game;
    
    delete window;
}
