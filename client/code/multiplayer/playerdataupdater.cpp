#include "../shader/shader.hpp"

#include "../global/globaluse.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "../debug/debugdrawer.hpp"

#include "../world/raytracer.hpp"
#include "../world/constrainthandler.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

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

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "physicsobjectdataparser.hpp"
#include "playerdataupdater.hpp"
        
PlayerDataUpdater::PlayerDataUpdater() 
{
    objParser = new PhysicsObjectDataParser();
    timeStamp = 0;
}

void PlayerDataUpdater::collect(string info)
{
    XMLDocument playerDataUpdaterDoc;
    
    playerDataUpdaterDoc.Parse(info.c_str());

    XMLElement* timeElem = playerDataUpdaterDoc.FirstChildElement("time");
    timeElem->QueryUnsignedAttribute("time", &timeStamp);

    /* root */
    XMLNode* root = playerDataUpdaterDoc.FirstChildElement("Soldiers");

    if (!root)
    {
        playerDataUpdaterDoc.PrintError();
        cout << info << endl << endl;
        throw runtime_error("ERROR::PlayerDataUpdater::collect() failed to load XML");
    }

    XMLElement* soldierElem = root->FirstChildElement("soldier");

    while (soldierElem)
    {
        /* playerID */
        int playerID;

        soldierElem->QueryIntAttribute("id", &playerID);

        playerIDs.push_back(playerID);
        
        /* speed */
        XMLElement* speedElem = soldierElem->FirstChildElement("speed");

        if (speedElem)
        {
            float speed = 1.0;
            speedElem->QueryFloatAttribute("speed", &speed);

            speeds.insert({playerID, speed});
        }
        
        /* health */
        XMLElement* healthElem = soldierElem->FirstChildElement("health");

        if (healthElem)
        {
            int health = 1.0;
            healthElem->QueryIntAttribute("health", &health);

            healths.insert({playerID, health});
        }

        /* moveDirection */
        XMLElement* moveDirectionElem = soldierElem->FirstChildElement("dir");

        if (moveDirectionElem)
        {
            vec3 moveDirection;

            moveDirectionElem->QueryFloatAttribute("x", &moveDirection.x);
            moveDirectionElem->QueryFloatAttribute("y", &moveDirection.y);
            moveDirectionElem->QueryFloatAttribute("z", &moveDirection.z);

            moveDirections.insert({playerID, moveDirection});
        }

        /* obj */
        XMLElement* objElem = soldierElem->FirstChildElement("obj");

        if (objElem)
        {
            objParser->parse(objElem);
        }

        /* weapons */
        XMLElement* armoryElem = soldierElem->FirstChildElement("armory");

        if (armoryElem)
        {
            vector < string > picked;

            XMLElement* weaponElem = armoryElem->FirstChildElement("weapon");

            while (weaponElem)
            {
                string name = weaponElem->GetText();

                picked.push_back(name);

                weaponElem = weaponElem->NextSiblingElement();
            }

            pickedWeapons.insert({playerID, picked});
        }

        soldierElem = soldierElem->NextSiblingElement();
    }
}

void PlayerDataUpdater::updateData(Player* player, bool interpolation, map < string, GameObject* > gameObjects)
{
    int playerID = player->getID();

    interpolation = player->isActive() ? false : interpolation;

    objParser->updatePhysicsObject(player->getGameObject(), interpolation, timeStamp);

    if (speeds.find(playerID) != speeds.end())
    {
        player->setSpeed(speeds[playerID]);
    }

    if (moveDirections.find(playerID) != moveDirections.end())
    {
        player->updateModel(moveDirections[playerID]);
        player->updateAnimation(moveDirections[playerID]);
    }

    Soldier* soldier = dynamic_cast < Soldier* >(player);

    if (!soldier)
    {
        return;
    }

    if (healths.find(playerID) != healths.end())
    {
        soldier->setHealth(healths[playerID]);
    }

    if (pickedWeapons.empty() || gameObjects.empty())
    {
        return;
    }

    for (int j = int(pickedWeapons[playerID].size()) - 1; j >= 0; j--)
    {
        GameObject* gameObject = gameObjects.find(pickedWeapons[playerID][j])->second;
        Weapon* weapon = dynamic_cast < Weapon* >(gameObject);

        if (!weapon)
        {
            throw(runtime_error("ERROR::WeaponPickerUpdater::updateData() gameObject is not a weapon"));
        }

        soldier->pick(weapon);
    }
}

void PlayerDataUpdater::updateData(vector < Player* > players, bool interpolation, map < string, GameObject* > gameObjects)
{
    for (size_t i = 0; i < playerIDs.size(); i++)
    {
        int playerID = playerIDs[i];
        Player* player = nullptr;

        for (size_t j = 0; j < players.size(); j++)
        {
            if (players[j]->getID() == playerID)
            {
                player = players[j];
                break;
            }
        }
        
        objParser->updatePhysicsObject(player->getGameObject(), interpolation, timeStamp);

        if (speeds.find(playerID) != speeds.end())
        {
            player->setSpeed(speeds[playerID]);
        }

        if (moveDirections.find(playerID) != moveDirections.end())
        {
            player->updateModel(moveDirections[playerID]);
            player->updateAnimation(moveDirections[playerID]);
        }

        Soldier* soldier = dynamic_cast < Soldier* >(player);

        if (!soldier)
        {
            return;
        }

        if (healths.find(playerID) != healths.end())
        {
            soldier->setHealth(healths[playerID]);
        }

        if (pickedWeapons.empty() || gameObjects.empty())
        {
            continue;
        }

        for (int j = int(pickedWeapons[playerID].size()) - 1; j >= 0; j--)
        {
            GameObject* gameObject = gameObjects.find(pickedWeapons[playerID][j])->second;
            Weapon* weapon = dynamic_cast < Weapon* >(gameObject);

            if (!weapon)
            {
                throw(runtime_error("ERROR::WeaponPickerUpdater::updateData() gameObject is not a weapon"));
            }

            soldier->pick(weapon);
        }
    }
}

int PlayerDataUpdater::getPlayerID(int index) const
{
    return playerIDs[index];
}

vector < int > PlayerDataUpdater::getPlayerIDs() const
{
    return playerIDs;
}

void PlayerDataUpdater::clear()
{
    playerIDs.clear();
    speeds.clear();
    moveDirections.clear();
    objParser->clear();
    pickedWeapons.clear();
    healths.clear();

    timeStamp = 0;
}

PlayerDataUpdater::~PlayerDataUpdater() 
{
    delete objParser;
}
