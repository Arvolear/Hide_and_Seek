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
#include "multiplayer.hpp"



    playerDataCollector = new PlayerDataCollector();
    client = new Client("159.224.87.241", 5040);
    
    if (client->getID() < 0)
    {
        throw(runtime_error("ERROR::Game() bad playerID"));
    }
    else
    {
        cout << "\nConnected!\nPlayerID: " << client->getID() << endl << endl;

        level->setPlayerID(client->getID());
        playerDataCollector->setPlayerID(client->getID());
    }
        
playerDataCollector->collect(level->getPlayer());
        cout << playerDataCollector->getData() << endl;

        client->sendMSG(playerDataCollector->getData());
