#include "../global/globaluse.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "../level/spawner.hpp"
#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "../multiplayer/node.hpp"
#include "../multiplayer/playerdatacollector.hpp"
#include "../multiplayer/playerdataupdater.hpp"
#include "../multiplayer/physicsobjectdatacollector.hpp"
#include "../multiplayer/physicsobjectdataupdater.hpp"
#include "../multiplayer/weapondatacollector.hpp"
#include "../multiplayer/weaponpickercollector.hpp"
#include "../multiplayer/weaponpickerupdater.hpp"
#include "../multiplayer/weapondroppercollector.hpp"
#include "../multiplayer/weapondropperupdater.hpp"
#include "../multiplayer/weaponfireupdater.hpp"
#include "../multiplayer/playerconnectioncollector.hpp"
#include "../multiplayer/playerdisconnectioncollector.hpp"
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

void Game::gameLoop()
{
    thread sender(&Multiplayer::broadcast, multiplayer);
    thread receiver(&Multiplayer::update, multiplayer);
        
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(16));

        physicsWorld->pollEvents();
        checkEvents();        

        if (global.getTime() > 1.0)
        {
            physicsWorld->updateSimulation(1.0, 1);
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
