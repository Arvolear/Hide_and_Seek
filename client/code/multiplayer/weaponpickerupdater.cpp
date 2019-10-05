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

#include "weaponpickerupdater.hpp"

WeaponPickerUpdater::WeaponPickerUpdater() 
{
    playerID = 0;
}

void WeaponPickerUpdater::collect(string info)
{
    XMLDocument weaponPickerUpdaterDoc;
    
    weaponPickerUpdaterDoc.Parse(info.data());

    /* root */
    XMLNode* root = weaponPickerUpdaterDoc.FirstChildElement("Pick");
    
    if (!root)
    {
        weaponPickerUpdaterDoc.PrintError();
        cout << info << endl << endl;
        throw runtime_error("ERROR::WeaponPickerUpdater::collect() failed to load XML");
    }

    /* playerID */
    XMLElement* playerIDElem = root->FirstChildElement("id");
    
    if (playerIDElem)
    {
        playerIDElem->QueryIntText(&playerID);
    }
    
    XMLElement* weaponsElem = root->FirstChildElement("wpns");

    /* name */
    XMLElement* nameElem = weaponsElem->FirstChildElement("name");

    while (nameElem)
    {
        string name = nameElem->GetText();

        names.push_back(name);

        nameElem = nameElem->NextSiblingElement();
    }
}

void WeaponPickerUpdater::updateData(Player* player, GameObject* gameObject)
{
    Soldier* soldier = dynamic_cast < Soldier* >(player);

    if (!soldier)
    {
        throw(runtime_error("ERROR::WeaponPickerUpdater::updateData() player is not a soldier"));
    }

    Weapon* weapon = dynamic_cast < Weapon* >(gameObject);

    if (!weapon)
    {
        throw(runtime_error("ERROR::WeaponPickerUpdater::updateData() gameObject is not a weapon"));
    }

    soldier->pick(weapon);
}

int WeaponPickerUpdater::getPlayerID() const
{
    return playerID;
}

vector < string > WeaponPickerUpdater::getNames() const
{
    return names;
}

void WeaponPickerUpdater::clear()
{
    playerID = 0;
    names.clear();
}

WeaponPickerUpdater::~WeaponPickerUpdater() {}
