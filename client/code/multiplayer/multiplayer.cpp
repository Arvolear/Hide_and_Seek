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

#include "../game_object/animation.hpp"
#include "../game_object/mesh.hpp"
#include "../game_object/bone.hpp"
#include "../game_object/skeleton.hpp"
#include "../game_object/viewfrustum.hpp"
#include "../game_object/boundsphere.hpp"
#include "../game_object/modelloader.hpp"
#include "../game_object/gameobject.hpp"

#include "../player/player.hpp"

#include "../level/dirlight.hpp"
#include "../level/skybox.hpp"
#include "../level/levelloader.hpp"
#include "../level/level.hpp"

#include "client.hpp"
#include "playerdatacollector.hpp"
#include "playerdataupdater.hpp"
#include "multiplayer.hpp"

Multiplayer::Multiplayer(Window* window, Level* level)
{
    client = new Client();
    playerDataCollector = new PlayerDataCollector();
    playerDataUpdater = new PlayerDataUpdater();

    this->window = window;
    this->level = level;

    playerID = 0;
}

void Multiplayer::connect()
{
    //client->connectToServer("159.224.87.241", 5040);
    client->connectToServer("127.0.0.1", 5040);

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
    }
    else
    {
        throw(runtime_error("ERROR::Multiplayer::connect() server is down"));
    }

    level->setPlayerID(playerID);
    playerDataCollector->setPlayerID(playerID);

    level->getPlayer()->setActive(true);

    cout << "PlayerID: " << playerID << endl;
}

void Multiplayer::broadcast()
{
    while (window->isOpen())
    {
        playerDataCollector->collect(level->getPlayer());

        client->sendMSG(playerDataCollector->getData());
    }
}

void Multiplayer::update()
{
    while (window->isOpen())
    {
        client->recvMSG(256);

        string msg = client->getMessage();

        //cout << msg << endl << msg.size() << endl;

        if (msg.find("Player") != string::npos)
        {    
            playerDataUpdater->collect(msg);
            playerDataUpdater->updateData(level->getPlayer(playerDataUpdater->getPlayerID()));
        }
    }
}

Multiplayer::~Multiplayer()
{
    delete client;
    delete playerDataCollector;
    delete playerDataUpdater;
}
