#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/shadowbuffer.hpp"
#include "../framebuffer/gbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../global/radialblur.hpp"
#include "../global/gaussianblur.hpp"
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

#include "../level/dirlightsoftshadow.hpp"
#include "../level/dirlight.hpp"
#include "../level/ssao.hpp"
#include "../level/atmosphere.hpp"
#include "../level/skybox.hpp"
#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "client.hpp"
#include "physicsobjectdataparser.hpp"
#include "playerdatacollector.hpp"
#include "playerdataupdater.hpp"
#include "gameobjectdatacollector.hpp"
#include "gameobjectdataupdater.hpp"
#include "weapondataupdater.hpp"
#include "weaponpickercollector.hpp"
#include "weaponpickerupdater.hpp"
#include "weapondroppercollector.hpp"
#include "weapondropperupdater.hpp"
#include "playerconnectionupdater.hpp"
#include "playerdisconnectionupdater.hpp"
#include "multiplayer.hpp"

Multiplayer::Multiplayer(Window* window, Level* level, World* world)
{
    client = new Client();
    playerDataCollector = new PlayerDataCollector();
    playerDataUpdater = new PlayerDataUpdater();
    gameObjectDataCollector = new GameObjectDataCollector();
    gameObjectDataUpdater = new GameObjectDataUpdater();
    weaponDataUpdater = new WeaponDataUpdater();
    weaponPickerCollector = new WeaponPickerCollector();
    weaponPickerUpdater = new WeaponPickerUpdater();
    weaponDropperCollector = new WeaponDropperCollector();
    weaponDropperUpdater = new WeaponDropperUpdater();
    playerConnectionUpdater = new PlayerConnectionUpdater();
    playerDisconnectionUpdater = new PlayerDisconnectionUpdater();

    this->window = window;
    this->level = level;
    this->world = world;

    playerID = 0;
}

void Multiplayer::connect()
{
    //client->connectToServer("159.224.87.241", 5040);
    //client->connectToServer("192.168.0.145", 5040);
    //client->connectToServer("192.168.0.183", 5040);
    client->connectToServer("127.0.0.1", 5040);
    
    /* connection info */
    client->recvMSG(100);
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
    }
    else
    {
        throw(runtime_error("ERROR::Multiplayer::connect() server is down"));
    }    

    /* gameObjects info */
    client->recvMSG(10000);
    msg = client->getMessage();

    if (msg != "")
    {
        gameObjectDataUpdater->collect(msg);
        gameObjectDataUpdater->updateData(level->getGameObjects(), false);
        gameObjectDataUpdater->clear();
    }
    else
    {
        throw(runtime_error("ERROR::Multiplayer::connect() not enough buffer (gameObject)"));
    }
    
    /* weapons info */
    client->recvMSG(1000);
    msg = client->getMessage();

    if (msg != "")
    {
        weaponDataUpdater->collect(msg);
        weaponDataUpdater->updateData(level->getGameObjects(), false);
        weaponDataUpdater->clear();
    }
    else
    {
        throw(runtime_error("ERROR::Multiplayer::connect() not enough buffer (weapon)"));
    }
    
    /* players info */
    client->recvMSG(1000);
    msg = client->getMessage();

    if (msg != "")
    {
        map < string, GameObject* > gameObjects = level->getGameObjects();
       
        playerDataUpdater->collect(msg);

        playerDataUpdater->updateData(level->getPlayers(), false, gameObjects);
        playerDataUpdater->clear();
    }
    else
    {
        throw(runtime_error("ERROR::Multiplayer::connect() not enough buffer (player)"));
    }
   
    level->setPlayerID(playerID);
    playerDataCollector->setPlayerID(playerID);
    gameObjectDataCollector->setSenderID(playerID);
    weaponPickerCollector->setPlayerID(playerID);
    weaponDropperCollector->setPlayerID(playerID);

    level->getConnectedPlayer()->setConnected(true);
    level->getConnectedPlayer()->setActive(true);

    cout << "PlayerID: " << playerID << endl;
}

void Multiplayer::broadcast()
{
    while (window->isOpen())
    {
        this_thread::sleep_for(chrono::milliseconds(50));

        /* player */
        if (level->getConnectedPlayer()->getGameObject()->getPhysicsObject()->getRigidBody()->getLinearVelocity().length() > 0.01)
        {
            playerDataCollector->collect(level->getConnectedPlayer());
            client->sendMSG(playerDataCollector->getData());
            playerDataCollector->clear();
        }

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

                    if (i.second->isCollidable() && RB->isActive() && !RB->isStaticOrKinematicObject())
                    {
                        gameObjectDataCollector->collect(i.second); 
                        client->sendMSG(gameObjectDataCollector->getData());
                        gameObjectDataCollector->clear();
                    }
                }
            }
        }

        /* pick */
        weaponPickerCollector->collect(level->getConnectedPlayer());
        client->sendMSG(weaponPickerCollector->getData());
        weaponPickerCollector->clear();
        
        /* drop */
        weaponDropperCollector->collect(level->getConnectedPlayer());
        client->sendMSG(weaponDropperCollector->getData());
        weaponDropperCollector->clear();
    }
}

void Multiplayer::update()
{
    while (window->isOpen())
    {
        string msg = "";
       
        while (window->isOpen())
        {
            client->recvMSG(300);    
            msg = client->getMessage();

            if (msg != "")
            {
                break;
            }

            this_thread::sleep_for(chrono::milliseconds(10));
        }

        if (msg.find("Con") != string::npos)
        {
            playerConnectionUpdater->collect(msg);

            vector < int > playerIDs = playerConnectionUpdater->getPlayerIDs();

            for (size_t i = 0; i < playerIDs.size(); i++)
            {
                playerConnectionUpdater->updateData(level->getIDPlayer(playerIDs[i]));
            }

            playerConnectionUpdater->clear();
        }
        else if (msg.find("Soldiers") != string::npos)
        { 
            playerDataUpdater->collect(msg);
            playerDataUpdater->updateData(level->getIDPlayer(playerDataUpdater->getPlayerID()), true);
            playerDataUpdater->clear();
        }
        else if (msg.find("Objs") != string::npos)
        {
            gameObjectDataUpdater->collect(msg);
            gameObjectDataUpdater->updateData(level->getGameObjects(), true);
            gameObjectDataUpdater->clear();
        }
        else if (msg.find("Pick") != string::npos)
        {
            weaponPickerUpdater->collect(msg);
            Player* player = level->getIDPlayer(weaponPickerUpdater->getPlayerID());
            vector < string > names = weaponPickerUpdater->getNames();

            for (size_t i = 0; i < names.size(); i++)
            {
                GameObject* gameObject = level->getGameObject(names[i]);

                weaponPickerUpdater->updateData(player, gameObject);
            }

            weaponPickerUpdater->clear();
        }
        else if (msg.find("Drop") != string::npos)
        {
            weaponDropperUpdater->collect(msg);
            Player* player = level->getIDPlayer(weaponDropperUpdater->getPlayerID());
            vector < string > names = weaponDropperUpdater->getNames();

            for (size_t i = 0; i < names.size(); i++)
            {
                GameObject* gameObject = level->getGameObject(names[i]);

                weaponDropperUpdater->updateData(player, gameObject);
            }

            weaponDropperUpdater->clear();
        }
        else if (msg.find("Dis") != string::npos)
        { 
            playerDisconnectionUpdater->collect(msg);

            vector < int > playerIDs = playerDisconnectionUpdater->getPlayerIDs();

            for (size_t i = 0; i < playerIDs.size(); i++)
            {
                playerDisconnectionUpdater->updateData(level->getIDPlayer(playerIDs[i]));
            }

            playerDisconnectionUpdater->clear();
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
    delete weaponDataUpdater;
    delete weaponPickerCollector;
    delete weaponPickerUpdater;
    delete weaponDropperCollector;
    delete weaponDropperUpdater;
    delete playerConnectionUpdater;
    delete playerDisconnectionUpdater;
}
