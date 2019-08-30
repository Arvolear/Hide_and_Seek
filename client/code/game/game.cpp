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
#include "../global/gaussianblur.cpp"

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

#include "../level/dirlight.hpp"
#include "../level/skybox.hpp"
#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "../multiplayer/client.hpp"
#include "../multiplayer/playerdatacollector.hpp"
#include "../multiplayer/playerdataupdater.hpp"
#include "../multiplayer/multiplayer.hpp"

#include "game.hpp"

Game::Game(Window* window, string levelName)
{
    this->window = window;
    this->mode = PLAY;

    window->hideCursor();

    level = new Level(window);
    level->loadLevel(levelName);

    gameShader = new Shader();

    gameBuffer = new ColorBuffer();
    quad = new RenderQuad();

    gaussianBlur = new GaussianBlur < ColorBuffer >();
    gaussianBlur->genBuffer(window->getRenderSize(), 4);

    multiplayer = new Multiplayer(window, level);
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
            level->getPlayer()->resetPrevCoords();
            
            mode = PLAY;
        }
    }
    
    /* draw debug spheres */
    if (window->isKeyPressedOnce(GLFW_KEY_C))
    {
        level->toggleDebug();
    }
}

void Game::init()
{
    gameShader->loadShaders(path("code/shader/gameShader.vert"), path("code/shader/gameShader.frag"));
    gameBuffer->genBuffer(window->getRenderSize());

    quad->init();

    /* multiplayer */
    multiplayer->connect();
}
        
void Game::gameLoop()
{
    init();
        
    thread sender(&Multiplayer::broadcast, multiplayer);
    thread receiver(&Multiplayer::update, multiplayer);
        
    while (window->isOpen())
    {
        window->pollEvents();
        checkEvents();        

        level->updatePlayers(mode);
        level->render();

        /***********************************
         * GAMEBUFFER
         * */

        /* bloom */
        GLuint blured = gaussianBlur->blur(level->getRenderTexture(1), 2, 1);
        
        gameBuffer->use();
        gameBuffer->clear();
        
        gameShader->use();

        glActiveTexture(GL_TEXTURE0 + level->getRenderTexture(0));
        glBindTexture(GL_TEXTURE_2D, level->getRenderTexture(0));
        gameShader->setInt("scene", level->getRenderTexture(0));
        
        glActiveTexture(GL_TEXTURE0 + blured);
        glBindTexture(GL_TEXTURE_2D, blured);
        gameShader->setInt("blurBloom", blured);

        gameShader->setFloat("exposure", 1.0);

        quad->render(gameShader);
 
        //window->render(blured);
        window->render(gameBuffer->getTexture());
    }
        
    sender.join();
    receiver.join();
}

Game::~Game()
{
    delete level;

    delete gameShader;
    delete gameBuffer;
    delete quad;

    delete gaussianBlur;
    
    delete multiplayer;
}
