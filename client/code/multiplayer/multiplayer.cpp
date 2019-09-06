#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/depthcolorbuffer.hpp"
#include "../framebuffer/gbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../global/gaussianblur.hpp"

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
#include "../game_object/rifle.hpp"

#include "../player/player.hpp"

#include "../level/dirlight.hpp"
#include "../level/skybox.hpp"
#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "client.hpp"
#include "playerdatacollector.hpp"
#include "playerdataupdater.hpp"
#include "gameobjectdatacollector.hpp"
#include "gameobjectdataupdater.hpp"
#include "weaponpickercollector.hpp"
#include "weaponpickerupdater.hpp"
#include "multiplayer.hpp"

Multiplayer::Multiplayer(Window* window, Level* level, World* world)
{
    client = new Client();
    playerDataCollector = new PlayerDataCollector();
    playerDataUpdater = new PlayerDataUpdater();
    gameObjectDataCollector = new GameObjectDataCollector();
    gameObjectDataUpdater = new GameObjectDataUpdater();
    weaponPickerCollector = new WeaponPickerCollector();
    weaponPickerUpdater = new WeaponPickerUpdater();

    this->window = window;
    this->level = level;
    this->world = world;

    playerID = 0;
}

void Multiplayer::connect()
{
    //client->connectToServer("159.224.87.241", 5040);
    client->connectToServer("192.168.0.145", 5040);
    //client->connectToServer("127.0.0.1", 5040);

    client->recvMSG(1150);

    /* get playerID */
    string msg = client->getMessage();

    if (msg != "")
    {
        XMLDocument newConnectionDoc;
        newConnectionDoc.Parse(msg.data());

        /* connected */
        XMLElement* joinElem = newConnectionDoc.FirstChildElement("join");

        if (joinElem)
        {
            joinElem->QueryIntText(&playerID);
        }
            
        gameObjectDataUpdater->collect(msg);
        gameObjectDataUpdater->updateData(level->getGameObjects(), false);
        gameObjectDataUpdater->clear();
    }
    else
    {
        throw(runtime_error("ERROR::Multiplayer::connect() server is down"));
    }

    level->setPlayerID(playerID);
    playerDataCollector->setPlayerID(playerID);
    gameObjectDataCollector->setSenderID(playerID);
    weaponPickerCollector->setPlayerID(playerID);

    level->getPlayer()->setActive(true);

    cout << "PlayerID: " << playerID << endl;
}

void Multiplayer::broadcast()
{
    while (window->isOpen())
    {
        /* player */
        playerDataCollector->collect(level->getPlayer());
        client->sendMSG(playerDataCollector->getData());
        playerDataCollector->clear();

        map < string, GameObject* > gameObjects = level->getGameObjects();
        vector < Player* > players = level->getPlayers();

        // erase players
        for (size_t i = 0; i < players.size(); i++)
        {
            gameObjects.erase(players[i]->getGameObject()->getName());
        }

        // game objects
        for (auto& i: gameObjects)
        {
            if (i.second->getPhysicsObject())
            {
                PhysicsObject* PO = i.second->getPhysicsObject();

                if (PO->getRigidBody())
                {
                    btRigidBody* RB = PO->getRigidBody();

                    btVector3 linearVel = RB->getLinearVelocity();

                    if (i.second->isCollidable() && RB->isActive() && !RB->isStaticOrKinematicObject() && linearVel.length() > 0.05)
                    {
                        gameObjectDataCollector->collect(i.second); 
                        client->sendMSG(gameObjectDataCollector->getData());
                        gameObjectDataCollector->clear();
                    }
                }
            }
        }

        /* pick */
        weaponPickerCollector->collect(level->getPlayer());

        client->sendMSG(weaponPickerCollector->getData());
        weaponPickerCollector->clear();
    }
}

void Multiplayer::update()
{
    while (window->isOpen())
    {
        client->recvMSG(300);

        string msg = client->getMessage();

        //cout << msg << endl << msg.size() << endl;

        if (msg.find("Player") != string::npos)
        { 
            playerDataUpdater->collect(msg);
            playerDataUpdater->updateData(level->getPlayer(playerDataUpdater->getPlayerID()));
            playerDataUpdater->clear();
        }
        else if (msg.find("Objs") != string::npos)
        {
            gameObjectDataUpdater->collect(msg);
            gameObjectDataUpdater->updateData(level->getGameObjects(), false);
            gameObjectDataUpdater->clear();
        }
        else if (msg.find("Pick") != string::npos)
        {
            cout << msg << endl;

            weaponPickerUpdater->collect(msg);
            Player* player = level->getPlayer(weaponPickerUpdater->getPlayerID());
            vector < string > names = weaponPickerUpdater->getNames();

            for (size_t i = 0; i < names.size(); i++)
            {
                GameObject* gameObject = level->getGameObject(names[i]);

                weaponPickerUpdater->updateData(player, gameObject);
            }

            weaponPickerUpdater->clear();
        }
    }
}

Multiplayer::~Multiplayer()
{
    delete client;
    delete playerDataCollector;
    delete playerDataUpdater;
    delete gameObjectDataCollector;
    delete gameObjectDataUpdater;
    delete weaponPickerCollector;
    delete weaponPickerUpdater;
}
