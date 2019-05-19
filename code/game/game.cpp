#include "../global/convert.hpp"
#include "../window/glfwevents.hpp"
#include "../global/shader.hpp"
#include "../window/framebuffer.hpp"
#include "../window/renderquad.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "game.hpp"

Game::Game(Window* window, string level)
{
    this->window = window;
    this->level = level;
    this->mode = PLAY;

    window->hideCursor();

    gameBuffer = new FrameBuffer();
    gameBuffer->genBuffer(window->getRenderSize());

    camera = new Camera(window, vec3(0, 5, 0), vec3(0, 0, -1));
    // ...
}

void Game::checkEvents()
{
    if (window->isKeyPressedOnce(GLFW_KEY_ESCAPE))
    {
        if (mode == PLAY)
        {
            window->showCursor();
            mode = ESCAPE;
        }
        else
        {
            window->hideCursor();
            mode = PLAY;
        }
    }
}

void Game::gameLoop()
{
    while (window->isOpen())
    {
        window->pollEvents();
        checkEvents();         
        
        if (mode == PLAY)
        {
            camera->update();
        }

        window->render(gameBuffer->getColorTexture());
    }
}

Game::~Game()
{
    delete gameBuffer;
    delete camera;
    // ...
}
