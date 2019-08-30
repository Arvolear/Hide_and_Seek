#include "../shader/shader.hpp"

#include "../global/convert.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

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

#include "../world/raytracer.hpp"

#include "../player/player.hpp"

#include "playerdataupdater.hpp"
        
PlayerDataUpdater::PlayerDataUpdater()
{
    playerID = 0;

    model = mat4(1.0);
    headCenter = vec3(0);
}

void PlayerDataUpdater::collect(string info)
{
    XMLDocument playerDataUpdaterDoc;
    
    playerDataUpdaterDoc.Parse(info.c_str());

    /* root */
    XMLNode* root = playerDataUpdaterDoc.FirstChildElement("Player");

    if (!root)
    {
        playerDataUpdaterDoc.PrintError();
        cout << info << endl << endl;
        return;
        //throw runtime_error("ERROR::PlayerDataUpdater::collect() failed to load XML");
    }

    /* playerID */
    XMLElement* playerIDElem = root->FirstChildElement("id");
    
    if (playerIDElem)
    {
        playerIDElem->QueryIntText(&playerID);
    }

    /* model */
    XMLElement* modelElem = root->FirstChildElement("mdl");

    if (modelElem)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                string str;
                str = char('a' + (i * 4 + j));

                modelElem->QueryFloatAttribute(str.data(), &model[i][j]);
            }
        }
    }

    /* headCenter */
    XMLElement* headCenterElem = root->FirstChildElement("hd");

    if (headCenterElem)
    {
        headCenterElem->QueryFloatAttribute("x", &headCenter.x);
        headCenterElem->QueryFloatAttribute("y", &headCenter.y);
        headCenterElem->QueryFloatAttribute("z", &headCenter.z);
    }
}

void PlayerDataUpdater::updateData(Player* player)
{
    player->getGameObject()->setModelTransform(model);
    player->setHeadCenter(headCenter);
}

int PlayerDataUpdater::getPlayerID() const
{
    return playerID;
}

PlayerDataUpdater::~PlayerDataUpdater() {}
