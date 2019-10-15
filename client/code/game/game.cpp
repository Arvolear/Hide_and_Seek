#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/shadowbuffer.hpp"
#include "../framebuffer/gbuffer.hpp"

#include "../window/glfwevents.hpp"
#include "../window/renderquad.hpp"
#include "../window/window.hpp"

#include "../global/radialblur.hpp"
#include "../global/gaussianblur.hpp"
#include "../global/gaussianblur.cpp"

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
#include "../game_object/weapon.hpp"
#include "../game_object/rifle.hpp"

#include "../player/player.hpp"

#include "../level/dirlightsoftshadow.hpp"
#include "../level/dirlight.hpp"
#include "../level/ssao.hpp"
#include "../level/atmosphere.hpp"
#include "../level/skybox.hpp"
#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "../multiplayer/client.hpp"
#include "../multiplayer/playerdatacollector.hpp"
#include "../multiplayer/playerdataupdater.hpp"
#include "../multiplayer/gameobjectdatacollector.hpp"
#include "../multiplayer/gameobjectdataupdater.hpp"
#include "../multiplayer/weaponpickercollector.hpp"
#include "../multiplayer/weaponpickerupdater.hpp"
#include "../multiplayer/weapondroppercollector.hpp"
#include "../multiplayer/weapondropperupdater.hpp"
#include "../multiplayer/playerconnectionupdater.hpp"
#include "../multiplayer/playerdisconnectionupdater.hpp"
#include "../multiplayer/multiplayer.hpp"

#include "game.hpp"

Game::Game(Window* window, string levelName)
{
    this->window = window;
    this->mode = PLAY;

    window->hideCursor();

    physicsWorld = new World();

    level = new Level(window, physicsWorld);
    level->loadLevel(levelName);

    gameShader = new Shader();

    gameBuffer = new ColorBuffer();
    quad = new RenderQuad();

    gaussianBlur = new GaussianBlur < ColorBuffer >();
    gaussianBlur->genBuffer(window->getRenderSize(), {GL_RGBA16F, GL_RGBA, GL_FLOAT}, 4);

    multiplayer = new Multiplayer(window, level, physicsWorld);

    global.fpsCounter->reset(window->getTime());
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
    
    /* PHYSICS EVENTS */
}

void Game::init()
{
    physicsWorld->createDebugDrawer();

    gameShader->loadShaders(global.path("code/shader/gameShader.vert"), global.path("code/shader/gameShader.frag"));
    gameBuffer->genBuffer(window->getRenderSize(), {{GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE}});

    quad->init();

    /* multiplayer */
    multiplayer->connect();
}
        
void Game::gameLoop()
{
    init();
        
    thread sender(&Multiplayer::broadcast, multiplayer);
    thread receiver(&Multiplayer::update, multiplayer);
        
    level->updateSunPos();
        
    while (window->isOpen())
    {
        window->pollEvents();
        physicsWorld->pollEvents();
        checkEvents();        

        if (global.fpsCounter->getFramesTime() > 0)
        {
            float step = 2.0 / (global.fpsCounter->getFPS());

            physicsWorld->updateSimulation(step, 1000);
        }

        //level->updateSunPos();
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

        glActiveTexture(GL_TEXTURE0);
        gameShader->setInt("scene", 0);
        glBindTexture(GL_TEXTURE_2D, level->getRenderTexture(0));
        
        glActiveTexture(GL_TEXTURE0 + 1);
        gameShader->setInt("blurBloom", 1);
        glBindTexture(GL_TEXTURE_2D, blured);

        gameShader->setFloat("exposure", 1.0);

        quad->render(gameShader);
 
        //window->render(blured);
        window->render(gameBuffer->getTexture());
        //window->render(level->getRenderTexture());
    
        global.fpsCounter->update(window->getTime());
        //cout << global.fpsCounter->getFPS() << endl;
    }
        
    sender.join();
    receiver.join();
}

Game::~Game()
{
    delete level;
    delete physicsWorld;

    delete gameShader;
    delete gameBuffer;
    delete quad;

    delete gaussianBlur;
    
    delete multiplayer;
}
