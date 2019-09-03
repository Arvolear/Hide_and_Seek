#include "../shader/shader.hpp"

#include "../global/convert.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "../debug/debugsphere.hpp"
#include "../debug/debugdrawer.hpp"

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

#include "../world/raytracer.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "playerdatacollector.hpp"
        
PlayerDataCollector::PlayerDataCollector()
{
    playerID = 0;

    model = mat4(1.0);
    moveDirection = vec3(0);
}

void PlayerDataCollector::setPlayerID(int playerID)
{
    this->playerID = playerID;
}

void PlayerDataCollector::collect(Player* player)
{
    if (player->getGameObject())
    {
        model = player->getGameObject()->getPhysicsObjectTransform();
        moveDirection = player->getMoveDirection();
    }
}

string PlayerDataCollector::getData() const
{
    XMLDocument playerDataCollectorDoc;

    /* root */
    XMLNode* root = playerDataCollectorDoc.NewElement("Player");

    playerDataCollectorDoc.InsertFirstChild(root);

    /* playerID */
    XMLElement* playerIDElem = playerDataCollectorDoc.NewElement("id");
    playerIDElem->SetText(playerID);

    root->InsertEndChild(playerIDElem);
    
    /* model */
    XMLElement* modelElem = playerDataCollectorDoc.NewElement("mdl");

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            string str;
            str = char('a' + (i * 4 + j));

            modelElem->SetAttribute(str.data(), model[i][j]);
        }
    }

    root->InsertEndChild(modelElem);

    /* moveDirection */
    XMLElement* moveDirectionElem = playerDataCollectorDoc.NewElement("dir");
    moveDirectionElem->SetAttribute("x", moveDirection.x);
    moveDirectionElem->SetAttribute("y", moveDirection.y);
    moveDirectionElem->SetAttribute("z", moveDirection.z);

    root->InsertEndChild(moveDirectionElem);

    /* printer */
    XMLPrinter playerDataCollectorPrinter;
    playerDataCollectorDoc.Print(&playerDataCollectorPrinter);

    string res("BEG\n");
    res += playerDataCollectorPrinter.CStr();
    res += "END\n";

    return res;
}

PlayerDataCollector::~PlayerDataCollector() {}
