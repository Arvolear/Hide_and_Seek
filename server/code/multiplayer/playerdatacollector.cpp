#include "../global/convert.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../world/raytracer.hpp"

#include "../player/player.hpp"

#include "playerdatacollector.hpp"
        
PlayerDataCollector::PlayerDataCollector()
{
    playerID = 0;
    model = nullptr;
}

void PlayerDataCollector::setPlayerID(int playerID)
{
    this->playerID = playerID;
}

void PlayerDataCollector::collect(Player* player)
{
    if (player->getPhysicsObject())
    {
        model = player->getPhysicsObject()->getTransform();
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

    for (int i = 0; i < 16; i++)
    {
        string str;
        str = char('a' + i);

        modelElem->SetAttribute(str.data(), cutFloat(model[i], 4));
    }

    root->InsertEndChild(modelElem);
    
    /* moveDirection */
    XMLElement* moveDirectionElem = playerDataCollectorDoc.NewElement("dir");
    moveDirectionElem->SetAttribute("x", cutFloat(moveDirection.x(), 4));
    moveDirectionElem->SetAttribute("y", cutFloat(moveDirection.y(), 4));
    moveDirectionElem->SetAttribute("z", cutFloat(moveDirection.z(), 4));

    root->InsertEndChild(moveDirectionElem);

    /* printer */
    XMLPrinter playerDataCollectorPrinter;
    playerDataCollectorDoc.Print(&playerDataCollectorPrinter);

    string res("BEG\n");
    res += playerDataCollectorPrinter.CStr();
    res += "END";

    return res;
}
        
void PlayerDataCollector::clear()
{
    playerID = 0;  

    delete[] model;
    model = nullptr;
}

PlayerDataCollector::~PlayerDataCollector() 
{
    delete[] model;
}
