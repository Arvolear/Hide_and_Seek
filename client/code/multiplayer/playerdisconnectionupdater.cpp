#include "../shader/shader.hpp"

#include "../global/globaluse.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

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
#include "../game_object/weapon.hpp"
#include "../game_object/rifle.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "playerdisconnectionupdater.hpp"
        
PlayerDisconnectionUpdater::PlayerDisconnectionUpdater() {}

void PlayerDisconnectionUpdater::collect(string info)
{
    XMLDocument playerDisconnectionUpdaterDoc;

    playerDisconnectionUpdaterDoc.Parse(info.c_str());

    /* root */
    XMLNode* root = playerDisconnectionUpdaterDoc.FirstChildElement("Dis");

    if (!root)
    {
        playerDisconnectionUpdaterDoc.PrintError();
        cout << info << endl << endl;
        throw runtime_error("ERROR::PlayerDisconnectionUpdater::collect() failed to load XML");
    }

    string str;
    int id;
    int i = 0;

    do 
    {
        str = char('a' + i);
        id = -1;

        dynamic_cast < XMLElement* >(root)->QueryIntAttribute(str.data(), &id);
        
        if (id == -1)
        {
            break;
        }

        playerIDs.push_back(id);
        i++;
    } 
    while (true);
}

void PlayerDisconnectionUpdater::updateData(Player* player)
{
    player->setConnected(false);
}

vector < int > PlayerDisconnectionUpdater::getPlayerIDs() const
{
    return playerIDs;
}

void PlayerDisconnectionUpdater::clear()
{
    playerIDs.clear();
}

PlayerDisconnectionUpdater::~PlayerDisconnectionUpdater() {}
