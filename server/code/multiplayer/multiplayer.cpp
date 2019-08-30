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
#include "multiplayer.hpp"

Multiplayer::Multiplayer(Level* level)
{
    node = new Node(5, 2, 5040);
    playerDataCollector = new PlayerDataCollector();
    playerDataUpdater = new PlayerDataUpdater();

    this->level = level;
}

void Multiplayer::broadcast()
{
    while (true)
    {
        /* player data */
        vector < Player* > players = level->getPlayers();

        for (size_t i = 0; i < players.size(); i++)
        {
            playerDataCollector->collect(players[i]);
            playerDataCollector->setPlayerID(i);

            node->sendMSG(-1, playerDataCollector->getData());

            //cout << i << endl << playerDataCollector->getData() << endl << playerDataCollector->getData().size() << endl << endl;

            playerDataCollector->clear();
        }
    }
}

void Multiplayer::update()
{
    while (true)
    {
        node->checkActivity(256);

        vector < string > messages = node->getMessages();

        /* player data */
        for (size_t i = 0; i < messages.size(); i++)
        {
            if (messages[i].find("Player") != string::npos)
            {
                //cout << i << messages[i] << endl << messages[i].size() << endl;

                playerDataUpdater->collect(messages[i]);
                playerDataUpdater->updateData(level->getPlayer(playerDataUpdater->getPlayerID()));
                playerDataUpdater->clear();
            }
        }
    }
}

Multiplayer::~Multiplayer()
{
    delete node;
    delete playerDataCollector;
    delete playerDataUpdater;
}
