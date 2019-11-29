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
#include "weapondatacollector.hpp"
#include "weaponpickercollector.hpp"
#include "weaponpickerupdater.hpp"
#include "weapondroppercollector.hpp"
#include "weapondropperupdater.hpp"
#include "weaponfireupdater.hpp"
#include "playerconnectioncollector.hpp"
#include "playerdisconnectioncollector.hpp"
#include "multiplayer.hpp"

Multiplayer::Multiplayer(Level* level, World* world)
{
    node = new Node(5, 2, 5040);
    playerDataCollector = new PlayerDataCollector(5);
    playerDataUpdater = new PlayerDataUpdater();
    physicsObjectDataCollector = new PhysicsObjectDataCollector(5);
    physicsObjectDataUpdater = new PhysicsObjectDataUpdater();
    weaponDataCollector = new WeaponDataCollector(5);
    weaponPickerCollector = new WeaponPickerCollector(5);
    weaponPickerUpdater = new WeaponPickerUpdater(world);
    weaponDropperCollector = new WeaponDropperCollector(5);
    weaponDropperUpdater = new WeaponDropperUpdater();
    weaponFireUpdater = new WeaponFireUpdater(world);
    playerConnectionCollector = new PlayerConnectionCollector(5);
    playerDisconnectionCollector = new PlayerDisconnectionCollector(5);

    this->level = level;
    this->world = world;
}

void Multiplayer::broadcast()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(50));

        /* new clients */
        if (node->isNewClients())
        {
            vector < int > new_sockets = node->getNewClientSockets();

            /* connect players */
            for (size_t i = 0; i < new_sockets.size(); i++)
            {
                if (new_sockets[i] > 0)
                {
                    cout << "Join playerID: " << i << endl;
                    level->getPlayer(i)->getPhysicsObject()->setOwnerID(i);
                    level->getPlayer(i)->setConnected(true);
                    
                    string message = "BEG\n<join>" + to_string(i) + "</join>\nEND";
                    try
                    {
                        node->sendMSG(new_sockets[i], message);
                    }
                    catch(exception& ex) {}
                }
            }
           
            /* physics objects */
            physicsObjectDataCollector->collect(level->getNoPlayersAndTheirWeaponsPhysicsObjects());

            for (size_t i = 0; i < new_sockets.size(); i++)
            {
                if (new_sockets[i] > 0)
                {
                    string message = physicsObjectDataCollector->getMergedData(level->getLevelPath() + "/physics_object.xml", i); 

                    /* send here */
                    try
                    {
                        node->sendMSG(new_sockets[i], message);
                    }
                    catch(exception& ex) {}
                }
            }

            physicsObjectDataCollector->clear();

            /* weapons */
            weaponDataCollector->collect(level->getPhysicsObjects());
                    
            for (size_t i = 0; i < new_sockets.size(); i++)
            {
                if (new_sockets[i] > 0)
                {
                    string message = weaponDataCollector->getMergedData(level->getLevelPath() + "/weapon.xml", i);

                    /* send here */
                    try
                    {
                        node->sendMSG(new_sockets[i], message);
                    }
                    catch(exception& ex) {}
                }
            }

            weaponDataCollector->clear();

            /* players */
            for (size_t i = 0; i < new_sockets.size(); i++)
            {
                if (new_sockets[i] > 0)
                {
                    /* init player data */
                    vector < Player* > players = level->getPlayers();
                    playerDataCollector->collect(players);

                    string message = playerDataCollector->getMergedData(level->getLevelPath() + "/soldier.xml", i, true, true, true);

                    try
                    {
                        node->sendMSG(new_sockets[i], message);
                    }
                    catch(exception& ex) {}
                    
                    playerDataCollector->clear();
                }
            }

            /* newToClient */
            for (size_t i = 0; i < new_sockets.size(); i++)
            {
                if (new_sockets[i] > 0)
                {
                    node->newToClient(i);    
                }
            }

            /* show connected */
            vector < int > sockets = node->getClientSockets();

            for (size_t i = 0; i < sockets.size(); i++)
            {
                if (sockets[i] > 0)
                {
                    playerConnectionCollector->collect(sockets, i); 

                    try
                    {
                        node->sendMSG(sockets[i], playerConnectionCollector->getData(i));
                    }
                    catch(exception& ex) {}

                    playerConnectionCollector->clear();
                }
            }
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
                    node->sendMSG(sockets[i], playerDisconnectionCollector->getData(i));
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
                    level->clearNoPlayersAndTheirWeaponsOwner(i);
                    level->getPlayer(i)->setConnected(false);

                    playerDataCollector->clearLast(i);
                    physicsObjectDataCollector->clearLast(i);
                    weaponDataCollector->clearLast(i);
                    weaponPickerCollector->clearAllLast();
                    weaponDropperCollector->clearAllLast();
                    playerConnectionCollector->clearAllLast();
                    playerDisconnectionCollector->clearAllLast();

                    /* send here */
                    node->oldToNothing(i);
                }
            }
        }

        vector < Player* > players = level->getPlayers();

        /* player */
        for (size_t i = 0; i < players.size(); i++)
        {
            if (!players[i]->isConnected())
            {
                continue;
            }

            playerDataCollector->collect(players[i]);

            /* send position info */
            vector < int > sockets = node->getClientSockets();

            for (size_t j = 0; j < sockets.size(); j++)
            {
                try
                {
                    /* another player, pos + ... */
                    if ((int)j != players[i]->getID())
                    {
                        node->sendMSG(sockets[j], playerDataCollector->getData(j, true, true));
                    }
                    else /* this player, don't include pos */
                    {
                        node->sendMSG(sockets[j], playerDataCollector->getData(j, false, true));
                    }
                }
                catch(exception& ex) {}
            }

            playerDataCollector->clear();
        }

        /* pickWeapons */
        for (size_t i = 0; i < players.size(); i++)
        {
            weaponPickerCollector->collect(players[i]);

            vector < int > sockets = node->getClientSockets();

            for (size_t j = 0; j < sockets.size(); j++)
            {
                try
                {
                    string msg = weaponPickerCollector->getData(j);

                    if (msg != "")
                    {
                        weaponDropperCollector->clearAllLast();
                    }

                    node->sendMSG(sockets[j], msg);
                }
                catch(exception& ex) {}
            }

            weaponPickerCollector->clear();
        }

        /* dropWeapons */
        for (size_t i = 0; i < players.size(); i++)
        {
            weaponDropperCollector->collect(players[i]);

            vector < int > sockets = node->getClientSockets();

            for (size_t j = 0; j < sockets.size(); j++)
            {
                try
                {
                    string msg = weaponDropperCollector->getData(j);

                    if (msg != "")
                    {
                        weaponPickerCollector->clearAllLast();
                    }

                    node->sendMSG(sockets[j], msg);
                }
                catch(exception& ex) {}
            }

            weaponDropperCollector->clear();
        }

        /* physicsobject data */
        map < string, PhysicsObject* > physicsObjects = level->getNoPlayersAndTheirWeaponsPhysicsObjects();

        /* physics object */
        for (auto& i: physicsObjects)
        {
            if (i.second->isCollidable() && !i.second->getRigidBody()->isStaticOrKinematicObject() && (i.second->getRigidBody()->isActive() || i.second->getName().find("weapon") != string::npos))
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
                            node->sendMSG(sockets[j], physicsObjectDataCollector->getData(j));
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
        vector < string > messages;

        while (true)
        {
            node->checkActivity(300);

            messages = node->getMessages();

            if (!messages.empty())
            {
                break;
            }
        }

        /* player data */
        for (size_t i = 0; i < messages.size(); i++)
        {
            if (messages[i].find("Player") != string::npos)
            {
                playerDataUpdater->collect(messages[i]);
                Player* player = level->getPlayer(playerDataUpdater->getPlayerID());

                playerDataUpdater->updateData(player);

                playerDataUpdater->clear();
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
            else if (messages[i].find("Fire") != string::npos)
            {
                weaponFireUpdater->collect(messages[i]);
                Player* player = level->getPlayer(weaponFireUpdater->getPlayerID());

                weaponFireUpdater->updateData(player, level->getPhysicsObjects());

                weaponFireUpdater->clear();
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
    delete weaponDataCollector;
    delete weaponPickerCollector;
    delete weaponPickerUpdater;
    delete weaponDropperCollector;
    delete weaponDropperUpdater;
    delete weaponFireUpdater;
    delete playerConnectionCollector;
    delete playerDisconnectionCollector;
}
