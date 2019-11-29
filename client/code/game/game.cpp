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
#include "../global/poissondisk.hpp"

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
#include "../game_object/instancedgameobject.hpp"
#include "../game_object/weapon.hpp"
#include "../game_object/rifle.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "../level/bloom.hpp"
#include "../level/lensflare.hpp"
#include "../level/dirlightsoftshadow.hpp"
#include "../level/dirlight.hpp"
#include "../level/ssao.hpp"
#include "../level/atmosphere.hpp"
#include "../level/skybox.hpp"
#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "../multiplayer/client.hpp"
#include "../multiplayer/physicsobjectdataparser.hpp"
#include "../multiplayer/playerdatacollector.hpp"
#include "../multiplayer/playerdataupdater.hpp"
#include "../multiplayer/gameobjectdatacollector.hpp"
#include "../multiplayer/gameobjectdataupdater.hpp"
#include "../multiplayer/weapondataupdater.hpp"
#include "../multiplayer/weaponpickercollector.hpp"
#include "../multiplayer/weaponpickerupdater.hpp"
#include "../multiplayer/weapondroppercollector.hpp"
#include "../multiplayer/weapondropperupdater.hpp"
#include "../multiplayer/weaponfirecollector.hpp"
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

    bloom = new Bloom();
    lensFlare = new LensFlare();

    gameShader = new Shader();

    gameBuffer = new ColorBuffer();
    quad = new RenderQuad();


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
            level->getConnectedPlayer(true)->resetViewRotation();
            
            //cout << dynamic_cast < Soldier* >(level->getConnectedPlayer())->getHealth() << endl;
            
            mode = ESCAPE;
        }
        else
        {
            window->hideCursor();
            level->getConnectedPlayer(true)->resetPrevCoords();
            
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
    
    /* draw debug spheres */
    if (window->isKeyPressedOnce(GLFW_KEY_P))
    {
        level->toggleVirtualPlayer();
    }
    
    /* PHYSICS EVENTS */
}

void Game::init()
{
    physicsWorld->createDebugDrawer();

    bloom->genBlurBuffer(window->getRenderSize(), 2);
    lensFlare->genBuffer(window->getRenderSize(), 2);
    lensFlare->loadHelperTextures(level->getLevelPath() + "/lens_flare");

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

        if (Global::fpsCounter->getFramesTime() > 0)
        {
            float step = 2.0 / (Global::fpsCounter->getFPS());

            /* lock mtx to block bodies removal */
            physicsWorld->updateSimulation(step, 1000);
        }

        level->updateSunPos();
        level->updatePlayers(mode);
        level->render();

        /***********************************
         * GAMEBUFFER
         * */
        
        /* bloom */
        bloom->setBloomTexture(level->getRenderTexture(1));
        bloom->blurBloom(2);

        /* lens flare */
        lensFlare->setBaseTexture(bloom->getTexture());
        lensFlare->renderFlares();
        
        /* render */
        gameBuffer->use();
        gameBuffer->clear();
        
        gameShader->use();
        
        gameShader->setFloat("exposure", 1.0);

        glActiveTexture(GL_TEXTURE0);
        gameShader->setInt("scene", 0);
        glBindTexture(GL_TEXTURE_2D, level->getRenderTexture(0));
         
        bloom->render(gameShader);
        lensFlare->render(gameShader);

        quad->render(gameShader);
 
        window->render(gameBuffer->getTexture());
    
        Global::fpsCounter->update(window->getTime());
        //cout << global.fpsCounter->getFPS() << endl;
    }
        
    sender.join();
    receiver.join();
}

Game::~Game()
{
    delete level;
    delete physicsWorld;

    delete bloom;
    delete lensFlare;

    delete gameShader;
    delete gameBuffer;
    delete quad;

    delete multiplayer;
}
