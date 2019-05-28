#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"

#include "../window/glfwevents.hpp"
#include "../window/renderquad.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

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

#include "../level/dirlight.hpp"
#include "../level/skybox.hpp"
#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "game.hpp"

Game::Game(Window* window, string levelName)
{
    this->window = window;
    this->levelName = levelName;
    this->mode = PLAY;

    window->hideCursor();

    gameBuffer = new ColorBuffer();
    gameBuffer->genBuffer(window->getRenderSize());

    level = new Level(window);
    level->loadLevel(levelName);

    /* TAKE THIS DATA FROM THE LEVEL */
    camera = level->getPlayer();
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

        /*gameBuffer->use();
        gameBuffer->clear();*/
        
        level->render();

        window->render(level->getRenderTexture());
    }
}

Game::~Game()
{
    delete gameBuffer;
    delete level;
    delete camera;
    // ...
}
