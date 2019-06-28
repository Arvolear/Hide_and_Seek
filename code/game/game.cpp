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

#include "../player/player.hpp"

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

    physicsWorld = new World();

    level = new Level(window, physicsWorld);
    level->loadLevel(levelName);

    player = level->getPlayer();
    testW = dynamic_cast < Weapon* >(level->getGameObject("testW"));
    // ...
}

void Game::checkEvents()
{
    /* menu */
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
            player->resetPrevCoords();
            mode = PLAY;
        }
    }
    
    /* Xray (physics) */
    if (window->isKeyPressedOnce(GLFW_KEY_X))
    {
        if (physicsWorld->getDebugDrawer()->getDebugMode() == 0)
        {
            physicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
        }
        else if (physicsWorld->getDebugDrawer()->getDebugMode() == btIDebugDraw::DBG_DrawWireframe)
        {
            physicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawAabb);
        }
        else
        {
            physicsWorld->getDebugDrawer()->setDebugMode(0);
        }
    }
    
    /* draw debug spheres */
    if (window->isKeyPressedOnce(GLFW_KEY_C))
    {
        level->toggleDebug();
    }
    
    /* swap players */
    if (window->isKeyPressedOnce(GLFW_KEY_P))
    {
        level->swapPlayers();
        player = level->getPlayer();

        player->resetPrevCoords();
    }
    
    /* visible player */
    if (window->isKeyPressedOnce(GLFW_KEY_O))
    {
        player->setActive((player->isActive() + 1) % 2);
    }

    /* T E S T */
    if (window->isKeyPressedOnce(GLFW_KEY_R))
    {
        testW->reload(); 
    }
    
    if (window->isButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        testW->fire(); 
    }

    /* PHYSICS EVENTS */
}

void Game::gameLoop()
{
    physicsWorld->createDebugDrawer();

    while (window->isOpen())
    {
        window->pollEvents();
        physicsWorld->pollEvents();
        checkEvents();         

        /*gameBuffer->use();
          gameBuffer->clear();*/

        if (window->getTime() > 1)
        {
            physicsWorld->updateSimulation(window->getTime());
        }

        if (mode == PLAY)
        {
            player->update();
        }

        testW->updateStatus();

        if (player->getGameObject())
        {
            testW->updatePosition(player->getPosition(), player->getForward(), player->getUp());
            testW->updateRotation(player->getHorizontalViewRotation());
            testW->updateRotation(player->getVerticalViewRotation());
        }
        
        level->render();

        window->render(level->getRenderTexture());
    }
}

Game::~Game()
{
    delete gameBuffer;

    delete level;

    delete physicsWorld;
}
