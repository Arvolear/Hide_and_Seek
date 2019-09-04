#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../player/player.hpp"

#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "node.hpp"
#include "playerdatacollector.hpp"
#include "playerdataupdater.hpp"
#include "physicsobjectdatacollector.hpp"
#include "physicsobjectdataupdater.hpp"
#include "multiplayer.hpp"

Multiplayer::Multiplayer(Level* level)
{
    node = new Node(5, 2, 5040);
    playerDataCollector = new PlayerDataCollector();
    playerDataUpdater = new PlayerDataUpdater();
    physicsObjectDataCollector = new PhysicsObjectDataCollector();
    physicsObjectDataUpdater = new PhysicsObjectDataUpdater();

    this->level = level;
}

void Multiplayer::broadcast()
{
    while (true)
    { 
        if (node->isNewClients())
        {
            physicsObjectDataCollector->collect(level->getPhysicsObjects());
            vector < int > new_sockets = node->getNewClientSockets();

            for (size_t i = 0; i < new_sockets.size(); i++)
            {
                if (new_sockets[i] > 0)
                {
                    string message = "BEG\n<join>" + to_string(i) + "</join>\n";
                    message += physicsObjectDataCollector->getData(true);
                    message += "END";

                    /* send here */
                    node->sendMSG(new_sockets[i], message);

                    node->newToOldClient(i);
                }
            }

            physicsObjectDataCollector->clear();
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

            playerDataCollector->collect(players[i]);
            playerDataCollector->setPlayerID(i);

            /* send position info */
            vector < int > sockets = node->getClientSockets();
            
            for (size_t j = 0; j < sockets.size(); j++)
            {
                if ((int)j != players[i]->getPhysicsObject()->getSenderID())
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
                    if ((int)j != i.second->getSenderID())
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
                playerDataUpdater->updateData(level->getPlayer(playerDataUpdater->getPlayerID()));
                playerDataUpdater->clear();
            }
            else if (messages[i].find("Obj") != string::npos)
            {
                physicsObjectDataUpdater->collect(messages[i]);

                PhysicsObject* physicsObject = level->getPhysicsObject(physicsObjectDataUpdater->getName());

                physicsObjectDataUpdater->updateData(physicsObject);
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
}
