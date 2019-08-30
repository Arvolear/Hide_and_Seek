#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../world/raytracer.hpp"

#include "../player/player.hpp"

#include "playerdataupdater.hpp"
        
PlayerDataUpdater::PlayerDataUpdater()
{
    playerID = 0;
    Up = Forward = btVector3(0, 0, 0);
    move = "";
}

void PlayerDataUpdater::collect(string info)
{
    //cout << info << endl << endl << endl;

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

    /* up */
    XMLElement* upElem = root->FirstChildElement("up");

    if (upElem)
    {
        float x, y, z;

        upElem->QueryFloatAttribute("x", &x);
        upElem->QueryFloatAttribute("y", &y);
        upElem->QueryFloatAttribute("z", &z);

        Up = btVector3(x, y, z);
    }

    /* forward */
    XMLElement* forwardElem = root->FirstChildElement("fwd");

    if (forwardElem)
    {
        float x, y, z;

        forwardElem->QueryFloatAttribute("x", &x);
        forwardElem->QueryFloatAttribute("y", &y);
        forwardElem->QueryFloatAttribute("z", &z);

        Forward = btVector3(x, y, z);
    }
    
    /* move */
    XMLElement* moveElem = root->FirstChildElement("mv");

    if (moveElem && moveElem->GetText())
    {
        move = moveElem->GetText();
    }
    else
    {
        move = "";
    }
}

void PlayerDataUpdater::updateData(Player* player)
{
    player->setUp(Up);
    player->setForward(Forward);
    player->setLeft(Up.cross(Forward));

    player->setMovement(move);
}

int PlayerDataUpdater::getPlayerID() const
{
    return playerID;
}
        
void PlayerDataUpdater::clear()
{
    playerID = 0;
    Up = Forward = btVector3(0, 0, 0);
    move = "";
}

PlayerDataUpdater::~PlayerDataUpdater() {}
