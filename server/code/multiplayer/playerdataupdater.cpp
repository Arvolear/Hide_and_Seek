#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../world/raytracer.hpp"

#include "../player/player.hpp"

#include "playerdataupdater.hpp"
        
PlayerDataUpdater::PlayerDataUpdater()
{
    playerID = 0;
    model = new btScalar[16];
    memset(model, 0, sizeof(btScalar) * 16);
    moveDirection = btVector3(0, 0, 0);
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
        throw runtime_error("ERROR::PlayerDataUpdater::collect() failed to load XML");
    }

    /* playerID */
    XMLElement* playerIDElem = root->FirstChildElement("id");
    
    if (playerIDElem)
    {
        playerIDElem->QueryIntText(&playerID);
    }

    /* model */
    XMLElement* modelElem = root->FirstChildElement("mdl");

    for (int i = 0; i < 16; i++)
    {
        string str;
        str = char('a' + i);

        modelElem->QueryFloatAttribute(str.data(), &model[i]);
    }
    
    /* moveDirection */
    XMLElement* moveDirectionElem = root->FirstChildElement("dir");
    
    if (moveDirectionElem)
    {
        float x, y, z;

        moveDirectionElem->QueryFloatAttribute("x", &x);
        moveDirectionElem->QueryFloatAttribute("y", &y);
        moveDirectionElem->QueryFloatAttribute("z", &z);

        moveDirection = btVector3(x, y, z);
    }
}

void PlayerDataUpdater::updateData(Player* player)
{
    player->getPhysicsObject()->setSenderID(playerID);
    player->getPhysicsObject()->setTransform(model);

    player->setMoveDirection(moveDirection);
}

int PlayerDataUpdater::getPlayerID() const
{
    return playerID;
}
        
void PlayerDataUpdater::clear()
{
    playerID = 0;
    memset(model, 0, sizeof(btScalar) * 16);
}

PlayerDataUpdater::~PlayerDataUpdater() 
{
    delete model;
}
