#include "../global/convert.hpp"
#include "../window/glfwevents.hpp"
#include "../global/shader.hpp"
#include "../window/framebuffer.hpp"
#include "../window/renderquad.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"
#include "../game/game.hpp"

#include "menu.hpp"

Menu::Menu()
{
    window = nullptr;

    menuShader = new Shader();
    menuBuffer = new FrameBuffer();

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
        game = new Game(window, "");
        game->gameLoop();
    }
}

void Menu::menuLoop()
{
    window = new Window();

    while (window->isOpen())
    {
        window->pollEvents();
        checkEvents();

        window->render(0);
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
