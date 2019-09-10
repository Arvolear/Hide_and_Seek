#include "../global/convert.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "../multiplayer/node.hpp"
#include "../multiplayer/playerdatacollector.hpp"
#include "../multiplayer/playerdataupdater.hpp"
#include "../multiplayer/physicsobjectdatacollector.hpp"
#include "../multiplayer/physicsobjectdataupdater.hpp"
#include "../multiplayer/weaponpickercollector.hpp"
#include "../multiplayer/weaponpickerupdater.hpp"
#include "../multiplayer/weapondroppercollector.hpp"
#include "../multiplayer/weapondropperupdater.hpp"
#include "../multiplayer/multiplayer.hpp"

#include "game.hpp"

Game::Game(string levelName)
{
    physicsWorld = new World();

    level = new Level(physicsWorld);
    level->loadLevel(levelName);
        
    multiplayer = new Multiplayer(level, physicsWorld);
}

void Game::checkEvents() {}

void Game::init()
{
    start = chrono::system_clock::now();
}

double Game::getTime()
{
    auto now = chrono::system_clock::now();

    chrono::duration < double > diff = now - start;
    
    return diff.count();
}
        
void Game::gameLoop()
{
    thread sender(&Multiplayer::broadcast, multiplayer);
    thread receiver(&Multiplayer::update, multiplayer);
        
    /* time */
    init();

    while (true)
    { 
        this_thread::sleep_for(chrono::milliseconds(10));

        physicsWorld->pollEvents();
        checkEvents();        

        if (getTime() > 1)
        {
            physicsWorld->updateSimulation(getTime());
        }

        level->update();
    }

    cout << "\n\nKILLING ALL THREADS\n\n";
    terminate();

    sender.join();
    receiver.join();
}

Game::~Game()
{
    delete level;
    delete physicsWorld;

    delete multiplayer;
}
