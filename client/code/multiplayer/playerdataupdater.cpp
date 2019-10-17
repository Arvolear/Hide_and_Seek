#include "../shader/shader.hpp"

#include "../global/globaluse.hpp"

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

#include "physicsobjectdataparser.hpp"
#include "playerdataupdater.hpp"
        
PlayerDataUpdater::PlayerDataUpdater() 
{
    objParser = new PhysicsObjectDataParser();
}

void PlayerDataUpdater::collect(string info)
{
    XMLDocument playerDataUpdaterDoc;
    
    playerDataUpdaterDoc.Parse(info.c_str());

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
            float speed;
            speedElem->QueryFloatAttribute("speed", &speed);

            speeds.insert({playerID, speed});
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

        objParser->parse(objElem);

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

    player->setSpeed(speeds[playerID]);
    player->updateModel(moveDirections[playerID]);
    player->updateAnimation(moveDirections[playerID]);

    objParser->updatePhysicsObject(player->getGameObject(), interpolation);

    Soldier* soldier = dynamic_cast < Soldier* >(player);

    if (!soldier || pickedWeapons.empty() || gameObjects.empty())
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

        player->setSpeed(speeds[playerID]);
        player->updateModel(moveDirections[playerID]);
        player->updateAnimation(moveDirections[playerID]);

        objParser->updatePhysicsObject(player->getGameObject(), interpolation);

        Soldier* soldier = dynamic_cast < Soldier* >(player);

        if (!soldier || pickedWeapons.empty() || gameObjects.empty())
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
}

PlayerDataUpdater::~PlayerDataUpdater() 
{
    delete objParser;
}
