#include "../global/convert.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../world/raytracer.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "playerdatacollector.hpp"
        
PlayerDataCollector::PlayerDataCollector() {}

void PlayerDataCollector::collect(Player* player)
{
    if (player->getPhysicsObject() && player->getPhysicsObject()->getOwnerID() >= 0)
    {
        playerIDs.push_back(player->getPhysicsObject()->getOwnerID());
        models.push_back(player->getPhysicsObject()->getTransform());
        moveDirections.push_back(player->getMoveDirection());

        Soldier* soldier = dynamic_cast < Soldier* >(player);

        if (soldier)
        {
            deque < Weapon* > weapons = soldier->getWeapons();
            vector < string > tmp;

            for (size_t i = 0; i < weapons.size(); i++)
            {
                tmp.push_back(weapons[i]->getName());
            }

            if (!tmp.empty())
            {
                pickedWeapons.push_back(tmp);
            }
        }
    }
}

void PlayerDataCollector::collect(vector < Player* > players)
{
    for (size_t i = 0; i < players.size(); i++)
    {
        if (players[i]->getPhysicsObject() && players[i]->getPhysicsObject()->getOwnerID() >= 0)
        {
            playerIDs.push_back(players[i]->getPhysicsObject()->getOwnerID());
            models.push_back(players[i]->getPhysicsObject()->getTransform());
            moveDirections.push_back(players[i]->getMoveDirection());

            Soldier* soldier = dynamic_cast < Soldier* >(players[i]);

            if (soldier)
            {
                deque < Weapon* > weapons = soldier->getWeapons();
                vector < string > tmp;

                for (size_t j = 0; j < weapons.size(); j++)
                {
                    tmp.push_back(weapons[j]->getName());
                }

                pickedWeapons.push_back(tmp);
            }
        }
    }
}

string PlayerDataCollector::getData(bool weapons, bool raw) const
{
    if (playerIDs.empty())
    {
        return "";
    }

    XMLDocument playerDataCollectorDoc;

    /* root */
    XMLNode* root = playerDataCollectorDoc.NewElement("Players");
    playerDataCollectorDoc.InsertFirstChild(root);

    for (size_t i = 0; i < playerIDs.size(); i++)
    {
        XMLElement* playerElem = playerDataCollectorDoc.NewElement("plr");

        /* playerID */
        XMLElement* playerIDElem = playerDataCollectorDoc.NewElement("id");
        playerIDElem->SetText(playerIDs[i]);

        playerElem->InsertEndChild(playerIDElem);

        /* model */
        XMLElement* modelElem = playerDataCollectorDoc.NewElement("mdl");

        for (int j = 0; j < 16; j++)
        {
            string str;
            str = char('a' + j);

            modelElem->SetAttribute(str.data(), cutFloat(models[i][j], 4));
        }

        playerElem->InsertEndChild(modelElem);

        /* moveDirection */
        XMLElement* moveDirectionElem = playerDataCollectorDoc.NewElement("dir");
        moveDirectionElem->SetAttribute("x", cutFloat(moveDirections[i].x(), 4));
        moveDirectionElem->SetAttribute("y", cutFloat(moveDirections[i].y(), 4));
        moveDirectionElem->SetAttribute("z", cutFloat(moveDirections[i].z(), 4));

        playerElem->InsertEndChild(moveDirectionElem);

        /* weapons */
        if (weapons && !pickedWeapons.empty())
        {
            for (size_t j = 0; j < pickedWeapons[i].size(); j++)
            {
                XMLElement* pickedElem = playerDataCollectorDoc.NewElement("pick");
                pickedElem->SetText(pickedWeapons[i][j].data());

                playerElem->InsertEndChild(pickedElem);
            }
        }

        root->InsertEndChild(playerElem);
    }

    /* printer */
    XMLPrinter playerDataCollectorPrinter;
    playerDataCollectorDoc.Print(&playerDataCollectorPrinter);

    string res = "";

    if (!raw)
    {
        res += "BEG\n";
        res += playerDataCollectorPrinter.CStr();
        res += "END";
    }
    else
    {
        res += playerDataCollectorPrinter.CStr();
    }

    return res; 
}

void PlayerDataCollector::clear()
{
    playerIDs.clear();
    moveDirections.clear();
    pickedWeapons.clear();

    for (size_t i = 0; i < models.size(); i++)
    {   
        delete[] models[i];
        models[i] = nullptr;
    }

    models.clear();
}

PlayerDataCollector::~PlayerDataCollector() 
{
    for (size_t i = 0; i < models.size(); i++)
    {   
        delete[] models[i];
    }
}
