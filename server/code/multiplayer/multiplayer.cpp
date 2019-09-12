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

#include "node.hpp"
#include "playerdatacollector.hpp"
#include "playerdataupdater.hpp"
#include "physicsobjectdatacollector.hpp"
#include "physicsobjectdataupdater.hpp"
#include "weaponpickercollector.hpp"
#include "weaponpickerupdater.hpp"
#include "weapondroppercollector.hpp"
#include "weapondropperupdater.hpp"
#include "playerconnectioncollector.hpp"
#include "playerdisconnectioncollector.hpp"
#include "multiplayer.hpp"

Multiplayer::Multiplayer(Level* level, World* world)
{
    node = new Node(5, 2, 5040);
    playerDataCollector = new PlayerDataCollector();
    playerDataUpdater = new PlayerDataUpdater();
    physicsObjectDataCollector = new PhysicsObjectDataCollector();
    physicsObjectDataUpdater = new PhysicsObjectDataUpdater();
    weaponPickerCollector = new WeaponPickerCollector();
    weaponPickerUpdater = new WeaponPickerUpdater(world);
    weaponDropperCollector = new WeaponDropperCollector();
    weaponDropperUpdater = new WeaponDropperUpdater();
    playerConnectionCollector = new PlayerConnectionCollector();
    playerDisconnectionCollector = new PlayerDisconnectionCollector();

    this->level = level;
    this->world = world;
}

void Multiplayer::broadcast()
{
    while (true)
    {
        /* new clients */
        if (node->isNewClients())
        {
            physicsObjectDataCollector->collect(level->getPhysicsObjects());
            vector < int > new_sockets = node->getNewClientSockets();

            for (size_t i = 0; i < new_sockets.size(); i++)
            {
                if (new_sockets[i] > 0)
                {
                    cout << "Join playerID: " << i << endl;
                    
                    /* connected */
                    level->getPlayer(i)->setConnected(true);

                    string message = "BEG\n<join>" + to_string(i) + "</join>\n";
                    message += physicsObjectDataCollector->getData(true);
                    message += "END";

                    /* send here */
                    node->sendMSG(new_sockets[i], message);

                    node->newToClient(i);
                    
                    level->getPlayer(i)->getPhysicsObject()->setOwnerID(i);
                    level->getPlayer(i)->setConnected(true);
                }
            }

            physicsObjectDataCollector->clear();
            
            /* show connected */
            vector < int > sockets = node->getClientSockets();

            for (size_t i = 0; i < sockets.size(); i++)
            {
                if (sockets[i] > 0)
                {
                    playerConnectionCollector->collect(sockets, i); 

                    try
                    {
                        node->sendMSG(sockets[i], playerConnectionCollector->getData());
                    }
                    catch(exception& ex) {}

                    playerConnectionCollector->clear();
                }
            }
        }

        /* player data */
        vector < Player* > players = level->getPlayers();
        /* physicsobject data */
        map < string, PhysicsObject* > physicsObjects = level->getPhysicsObjects();

        /* player */
        for (size_t i = 0; i < players.size(); i++)
        {
            /* erase PO */
            physicsObjects.erase(players[i]->getPhysicsObject()->getName());

            playerDataCollector->setPlayerID(i);
            playerDataCollector->collect(players[i]);

            /* send position info */
            vector < int > sockets = node->getClientSockets();

            for (size_t j = 0; j < sockets.size(); j++)
            {
                if ((int)j != players[i]->getPhysicsObject()->getOwnerID())
                {
                    try
                    {
                        node->sendMSG(sockets[j], playerDataCollector->getData());
                    }
                    catch(exception& ex) {}
                }
            }

            playerDataCollector->clear();
        }

        /* physics object */
        for (auto& i: physicsObjects)
        {
            if (i.second->isCollidable() && i.second->getRigidBody()->isActive() && !i.second->getRigidBody()->isStaticOrKinematicObject())
            {
                physicsObjectDataCollector->collect(i.second); 

                /* send position info */
                vector < int > sockets = node->getClientSockets();

                for (size_t j = 0; j < sockets.size(); j++)
                {
                    if ((int)j != i.second->getOwnerID())
                    {
                        try
                        {
                            node->sendMSG(sockets[j], physicsObjectDataCollector->getData());
                        }
                        catch(exception& ex) {}
                    }
                }

                physicsObjectDataCollector->clear();
            }
        }

        /* pickWeapons */
        for (size_t i = 0; i < players.size(); i++)
        {
            weaponPickerCollector->setPlayerID(i);
            weaponPickerCollector->collect(players[i]);

            vector < int > sockets = node->getClientSockets();

            for (size_t j = 0; j < sockets.size(); j++)
            {
                try
                {
                    node->sendMSG(sockets[j], weaponPickerCollector->getData());
                }
                catch(exception& ex) {}
            }

            weaponPickerCollector->clear();
        }
        
        /* dropWeapons */
        for (size_t i = 0; i < players.size(); i++)
        {
            weaponDropperCollector->setPlayerID(i);
            weaponDropperCollector->collect(players[i]);

            vector < int > sockets = node->getClientSockets();

            for (size_t j = 0; j < sockets.size(); j++)
            {
                try
                {
                    node->sendMSG(sockets[j], weaponDropperCollector->getData());
                }
                catch(exception& ex) {}
            }

            weaponDropperCollector->clear();
        }
        
        /* old clients */
        if (node->isOldClients())
        {
            /* hide old clients */
            vector < int > old_sockets = node->getOldClientSockets();
            vector < int > sockets = node->getClientSockets();

            playerDisconnectionCollector->collect(old_sockets);  

            for (size_t i = 0; i < sockets.size(); i++)
            {
                try
                {
                    node->sendMSG(sockets[i], playerDisconnectionCollector->getData());
                }
                catch(exception& ex) {}
            }

            playerDisconnectionCollector->clear();

            for (size_t i = 0; i < old_sockets.size(); i++)
            {
                if (old_sockets[i] > 0)
                {
                    cout << "Quit playerID: " << i << endl;
                    
                    /* disconnected */
                    level->getPlayer(i)->setConnected(false);

                    /* send here */
                    node->oldToNothing(i);
                }
            }
        }
    }
}

void Multiplayer::update()
{
    while (true)
    {
        node->checkActivity(300);

        vector < string > messages = node->getMessages();

        /* player data */
        for (size_t i = 0; i < messages.size(); i++)
        {
            if (messages[i].find("Player") != string::npos)
            {
                playerDataUpdater->collect(messages[i]);
                Player* player = level->getPlayer(playerDataUpdater->getPlayerID());

                if (playerDataUpdater->getPlayerID() == player->getPhysicsObject()->getOwnerID())
                {
                    playerDataUpdater->updateData(player);
                }

                playerDataUpdater->clear();
            }
            else if (messages[i].find("Obj") != string::npos)
            {
                physicsObjectDataUpdater->collect(messages[i]);
                PhysicsObject* physicsObject = level->getPhysicsObject(physicsObjectDataUpdater->getName());

                if (physicsObjectDataUpdater->getSenderID() == physicsObject->getOwnerID())
                {
                    physicsObjectDataUpdater->updateData(physicsObject);
                }

                physicsObjectDataUpdater->clear();
            }
            else if (messages[i].find("Pick") != string::npos)
            {
                weaponPickerUpdater->collect(messages[i]);
                Player* player = level->getPlayer(weaponPickerUpdater->getPlayerID());

                weaponPickerUpdater->updateData(player);

                weaponPickerUpdater->clear();
            }
            else if (messages[i].find("Drop") != string::npos)
            {
                weaponDropperUpdater->collect(messages[i]);
                Player* player = level->getPlayer(weaponDropperUpdater->getPlayerID());

                weaponDropperUpdater->updateData(player);

                weaponDropperUpdater->clear();
            }
        }
    }
}

Multiplayer::~Multiplayer()
{
    delete node;
    delete playerDataCollector;
    delete playerDataUpdater;
    delete physicsObjectDataCollector;
    delete physicsObjectDataUpdater;
    delete weaponPickerCollector;
    delete weaponPickerUpdater;
    delete weaponDropperCollector;
    delete weaponDropperUpdater;
    delete playerConnectionCollector;
    delete playerDisconnectionCollector;
}
