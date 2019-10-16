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
    if (player->getPhysicsObject())
    {
        playerIDs.push_back(player->getID());
        models.insert({player->getID(), player->getPhysicsObject()->getTransform()});
        moveDirections.insert({player->getID(), player->getMoveDirection()});

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
                pickedWeapons.insert({player->getID(), tmp});
            }
        }
    }
}

void PlayerDataCollector::collect(vector < Player* > players)
{
    for (size_t i = 0; i < players.size(); i++)
    {
        if (players[i]->getPhysicsObject())
        {
            playerIDs.push_back(players[i]->getID());
            models.insert({players[i]->getID(), players[i]->getPhysicsObject()->getTransform()});
            moveDirections.insert({players[i]->getID(), players[i]->getMoveDirection()});

            Soldier* soldier = dynamic_cast < Soldier* >(players[i]);

            if (soldier)
            {
                deque < Weapon* > weapons = soldier->getWeapons();
                vector < string > tmp;

                for (size_t j = 0; j < weapons.size(); j++)
                {
                    tmp.push_back(weapons[j]->getName());
                }

                if (!tmp.empty())
                {
                    pickedWeapons.insert({players[i]->getID(), tmp});
                }
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
    XMLNode* root = playerDataCollectorDoc.NewElement("Soldiers");
    playerDataCollectorDoc.InsertFirstChild(root);

    for (size_t i = 0; i < playerIDs.size(); i++)
    {
        XMLElement* soldierElem = playerDataCollectorDoc.NewElement("soldier");

        /* playerID */
        soldierElem->SetAttribute("id", playerIDs[i]);

        /* model */
        XMLElement* modelElem = playerDataCollectorDoc.NewElement("mdl");

        for (int j = 0; j < 16; j++)
        {
            string str;
            str = char('a' + j);

            modelElem->SetAttribute(str.data(), cutFloat(models[playerIDs[i]][j], 4));
        }

        soldierElem->InsertEndChild(modelElem);

        /* moveDirection */
        XMLElement* moveDirectionElem = playerDataCollectorDoc.NewElement("dir");
        moveDirectionElem->SetAttribute("x", cutFloat(moveDirections[playerIDs[i]].x(), 4));
        moveDirectionElem->SetAttribute("y", cutFloat(moveDirections[playerIDs[i]].y(), 4));
        moveDirectionElem->SetAttribute("z", cutFloat(moveDirections[playerIDs[i]].z(), 4));

        soldierElem->InsertEndChild(moveDirectionElem);

        /* weapons */
        if (weapons && !pickedWeapons.empty())
        {
            XMLElement* armoryElem = playerDataCollectorDoc.NewElement("armory");

            for (size_t j = 0; j < pickedWeapons[playerIDs[i]].size(); j++)
            {
                XMLElement* weaponElem = playerDataCollectorDoc.NewElement("weapon");
                weaponElem->SetText(pickedWeapons[playerIDs[i]][j].data());

                armoryElem->InsertEndChild(weaponElem);
            }

            soldierElem->InsertEndChild(armoryElem);
        }

        root->InsertEndChild(soldierElem);
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
        
string PlayerDataCollector::getMergedData(string fileName, bool weapons, bool raw) const
{
    if (playerIDs.empty())
    {
        return "";
    }

    XMLDocument playerDataCollectorDoc; 
    playerDataCollectorDoc.LoadFile(fileName.c_str());

    /* root */
    XMLNode* root = playerDataCollectorDoc.FirstChildElement("Soldiers");

    if (!root)
    {
        throw runtime_error("ERROR::PlayerDataCollector::getMergedData() failed to load XML");
    }

    XMLElement* soldierElem = root->FirstChildElement("soldier");

    while (soldierElem)
    {
        int id = 0;
        soldierElem->QueryIntAttribute("id", &id);

        if (find(playerIDs.begin(), playerIDs.end(), id) == playerIDs.end())
        {
            XMLElement* tmp = soldierElem;
            soldierElem = soldierElem->NextSiblingElement();
            root->DeleteChild(tmp);
            continue;
        }

        XMLElement* positionElem = soldierElem->FirstChildElement("pos");
        
        if (positionElem)
        {
            soldierElem->DeleteChild(positionElem);
        }

        XMLElement* rotationElem = soldierElem->FirstChildElement("rot");

        if (rotationElem)
        {
            soldierElem->DeleteChild(rotationElem);
        }
        
        XMLElement* armoryElem = soldierElem->FirstChildElement("armory");

        if (armoryElem)
        {
            soldierElem->DeleteChild(armoryElem);
        }
        
        XMLElement* objElem = soldierElem->FirstChildElement("obj");

        if (objElem)
        {
            XMLElement* positionElem = soldierElem->FirstChildElement("pos");

            if (positionElem)
            {
                objElem->DeleteChild(positionElem);
            }
            
            XMLElement* rotationElem = soldierElem->FirstChildElement("rot");

            if (rotationElem)
            {
                objElem->DeleteChild(rotationElem);
            }

            /* model */
            XMLElement* modelElem = playerDataCollectorDoc.NewElement("mdl");

            for (int j = 0; j < 16; j++)
            {
                string str;
                str = char('a' + j);

                modelElem->SetAttribute(str.data(), cutFloat(models[id][j], 4));
            }

            objElem->InsertEndChild(modelElem);
        }

        /* moveDirection */
        XMLElement* moveDirectionElem = playerDataCollectorDoc.NewElement("dir");
        moveDirectionElem->SetAttribute("x", cutFloat(moveDirections[id].x(), 4));
        moveDirectionElem->SetAttribute("y", cutFloat(moveDirections[id].y(), 4));
        moveDirectionElem->SetAttribute("z", cutFloat(moveDirections[id].z(), 4));

        soldierElem->InsertEndChild(moveDirectionElem);

        /* weapons */
        if (weapons && !pickedWeapons.empty())
        {
            XMLElement* armoryElem = playerDataCollectorDoc.NewElement("armory");

            for (size_t j = 0; j < pickedWeapons[id].size(); j++)
            {
                XMLElement* weaponElem = playerDataCollectorDoc.NewElement("weapon");
                weaponElem->SetText(pickedWeapons[id][j].data());

                armoryElem->InsertEndChild(weaponElem);
            }

            soldierElem->InsertEndChild(armoryElem);
        }

        soldierElem = soldierElem->NextSiblingElement();
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

    for (auto& i: models)
    {   
        delete[] i.second;
    }

    models.clear();
}

PlayerDataCollector::~PlayerDataCollector() 
{
    clear();
}
