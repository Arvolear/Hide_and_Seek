#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../world/raytracer.hpp"

#include "../player/player.hpp"

#include "playerdatacollector.hpp"
        
PlayerDataCollector::PlayerDataCollector()
{
    playerID = 0;
    model = new btScalar[16];
    memset(model, 0, 16 * sizeof(btScalar));
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
        headCenter = player->getHeadCenter();
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

        modelElem->SetAttribute(str.data(), model[i]);
    }

    root->InsertEndChild(modelElem);

    /* head center */
    XMLElement* headCenterElem = playerDataCollectorDoc.NewElement("hd");
    headCenterElem->SetAttribute("x", headCenter.x());
    headCenterElem->SetAttribute("y", headCenter.y());
    headCenterElem->SetAttribute("z", headCenter.z());
    
    root->InsertEndChild(headCenterElem);

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
    memset(model, 0, 16 * sizeof(btScalar));
}

PlayerDataCollector::~PlayerDataCollector() 
{
    delete[] model;
}
