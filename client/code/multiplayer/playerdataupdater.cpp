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

#include "playerdataupdater.hpp"
        
PlayerDataUpdater::PlayerDataUpdater() {}

void PlayerDataUpdater::collect(string info)
{
    XMLDocument playerDataUpdaterDoc;
    
    playerDataUpdaterDoc.Parse(info.c_str());

    /* root */
    XMLNode* root = playerDataUpdaterDoc.FirstChildElement("Players");

    if (!root)
    {
        playerDataUpdaterDoc.PrintError();
        cout << info << endl << endl;
        throw runtime_error("ERROR::PlayerDataUpdater::collect() failed to load XML");
    }

    XMLElement* playerElem = root->FirstChildElement("plr");

    while (playerElem)
    {
        int playerID;
        mat4 model;
        vec3 moveDirection;
        vector < string > picked;

        /* playerID */
        XMLElement* playerIDElem = playerElem->FirstChildElement("id");

        if (playerIDElem)
        {
            playerIDElem->QueryIntText(&playerID);
        }

        /* model */
        XMLElement* modelElem = playerElem->FirstChildElement("mdl");

        if (modelElem)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    string str;
                    str = char('a' + (i * 4 + j));

                    modelElem->QueryFloatAttribute(str.data(), &model[i][j]);
                }
            }
        }

        /* moveDirection */
        XMLElement* moveDirectionElem = playerElem->FirstChildElement("dir");

        if (moveDirectionElem)
        {
            moveDirectionElem->QueryFloatAttribute("x", &moveDirection.x);
            moveDirectionElem->QueryFloatAttribute("y", &moveDirection.y);
            moveDirectionElem->QueryFloatAttribute("z", &moveDirection.z);
        }

        /* weapons */
        XMLElement* pickedElem = playerElem->FirstChildElement("pick");

        while (pickedElem)
        {
            string name = pickedElem->GetText();

            picked.push_back(name);

            pickedElem = pickedElem->NextSiblingElement();
        }

        playerIDs.push_back(playerID);
        models.push_back(model);
        moveDirections.push_back(moveDirection);
        pickedWeapons.push_back(picked);

        playerElem = playerElem->NextSiblingElement();
    }
}

void PlayerDataUpdater::updateData(Player* player, bool interpolation, map < string, GameObject* > gameObjects)
{
    player->getGameObject()->setPhysicsObjectTransform(models[0], interpolation);

    player->updateModel(moveDirections[0]);
    player->updateAnimation(moveDirections[0]);

    Soldier* soldier = dynamic_cast < Soldier* >(player);

    if (!soldier || pickedWeapons.empty() || gameObjects.empty())
    {
        return;
    }

    for (int j = int(pickedWeapons[0].size()) - 1; j >= 0; j--)
    {
        GameObject* gameObject = gameObjects.find(pickedWeapons[0][j])->second;
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
        Player* player = players[playerIDs[i]];

        player->getGameObject()->setPhysicsObjectTransform(models[i], interpolation);

        player->updateModel(moveDirections[i]);
        player->updateAnimation(moveDirections[i]);

        Soldier* soldier = dynamic_cast < Soldier* >(player);

        if (!soldier || pickedWeapons.empty() || gameObjects.empty())
        {
            return;
        }

        for (int j = int(pickedWeapons[i].size()) - 1; j >= 0; j--)
        {
            GameObject* gameObject = gameObjects.find(pickedWeapons[i][j])->second;
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
    models.clear();
    moveDirections.clear();
    pickedWeapons.clear();
}

PlayerDataUpdater::~PlayerDataUpdater() {}
