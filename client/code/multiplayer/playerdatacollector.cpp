#include "../shader/shader.hpp"

#include "../global/convert.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "../debug/debugdrawer.hpp"

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

#include "../world/raytracer.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "playerdatacollector.hpp"
        
PlayerDataCollector::PlayerDataCollector()
{
    playerID = 0;

    model = mat4(1.0);
    moveDirection = vec3(0.0);
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

            modelElem->SetAttribute(str.data(), cutFloat(model[i][j], 4));
        }
    }

    root->InsertEndChild(modelElem);

    /* moveDirection */
    XMLElement* moveDirectionElem = playerDataCollectorDoc.NewElement("dir");
    moveDirectionElem->SetAttribute("x", cutFloat(moveDirection.x, 4));
    moveDirectionElem->SetAttribute("y", cutFloat(moveDirection.y, 4));
    moveDirectionElem->SetAttribute("z", cutFloat(moveDirection.z, 4));

    root->InsertEndChild(moveDirectionElem);

    /* printer */
    XMLPrinter playerDataCollectorPrinter;
    playerDataCollectorDoc.Print(&playerDataCollectorPrinter);

    string res("BEG\n");
    res += playerDataCollectorPrinter.CStr();
    res += "END\n";

    return res;
}

void PlayerDataCollector::clear()
{
    model = mat4(1.0);
    moveDirection = vec3(0.0);
}

PlayerDataCollector::~PlayerDataCollector() {}
