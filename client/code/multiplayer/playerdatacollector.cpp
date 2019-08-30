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

#include "playerdatacollector.hpp"
        
PlayerDataCollector::PlayerDataCollector()
{
    playerID = 0;

    Up = Forward = vec3(0);
}

void PlayerDataCollector::setPlayerID(int playerID)
{
    this->playerID = playerID;
}

void PlayerDataCollector::collect(Player* player)
{
    if (player->getGameObject())
    {
        Up = player->getUp();
        Forward = player->getForward();

        move = player->getMove();
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

    /* up */
    XMLElement* upElem = playerDataCollectorDoc.NewElement("up");
    upElem->SetAttribute("x", Up.x);
    upElem->SetAttribute("y", Up.y);
    upElem->SetAttribute("z", Up.z);
    
    root->InsertEndChild(upElem);
    
    /* forward */
    XMLElement* forwardElem = playerDataCollectorDoc.NewElement("fwd");
    forwardElem->SetAttribute("x", Forward.x);
    forwardElem->SetAttribute("y", Forward.y);
    forwardElem->SetAttribute("z", Forward.z);
    
    root->InsertEndChild(forwardElem);

    /* move */
    XMLElement* moveElem = playerDataCollectorDoc.NewElement("mv");
    moveElem->SetText(move.data());

    root->InsertEndChild(moveElem);
    
    /* printer */
    XMLPrinter playerDataCollectorPrinter;
    playerDataCollectorDoc.Print(&playerDataCollectorPrinter);

    string res("BEG\n");
    res += playerDataCollectorPrinter.CStr();
    res += "END\n";

    return res;
}

PlayerDataCollector::~PlayerDataCollector() {}
