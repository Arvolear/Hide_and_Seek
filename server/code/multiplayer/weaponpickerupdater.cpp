#include "../global/convert.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "weaponpickerupdater.hpp"

WeaponPickerUpdater::WeaponPickerUpdater(World* world)
{
    playerID = 0;
    this->world = world;
    rayTracer = new RayTracer(world->getWorld());

    pickFrom = pickTo = btVector3(0, 0, 0);

    weapon = nullptr;
}

void WeaponPickerUpdater::collect(string info)
{
    XMLDocument weaponPickerUpdaterDoc;
    
    weaponPickerUpdaterDoc.Parse(info.c_str());

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

    /* pickFrom */
    XMLElement* fromElem = root->FirstChildElement("frm");
    
    if (fromElem)
    {
        float x, y, z;

        fromElem->QueryFloatAttribute("x", &x);
        fromElem->QueryFloatAttribute("y", &y);
        fromElem->QueryFloatAttribute("z", &z);

        pickFrom = btVector3(x, y, z);
    }
    
    /* pickTo */
    XMLElement* toElem = root->FirstChildElement("to");
    
    if (toElem)
    {
        float x, y, z;

        toElem->QueryFloatAttribute("x", &x);
        toElem->QueryFloatAttribute("y", &y);
        toElem->QueryFloatAttribute("z", &z);

        pickTo = btVector3(x, y, z);
    }

    /* rayTracer */
    unique_ptr < RayResult > result(rayTracer->rayCast(pickFrom, pickTo, false));

    if (!result.get())
    {
        return;
    }

    float dist = (pickFrom - result->hitPoint).length();

    int optimalDistance = 20;

    if (dist > optimalDistance)
    {
        return;
    }

    weapon = dynamic_cast < Weapon* >(static_cast < PhysicsObject* >(result->body->getUserPointer()));
}

void WeaponPickerUpdater::updateData(Player* player)
{
    Soldier* soldier = dynamic_cast < Soldier* >(player);

    if (!soldier)
    {
        throw(runtime_error("ERROR::WeaponPickerUPdater::updatedata() player is not a soldier"));
    }

    if (weapon)
    {
        if (!weapon->getUserPointer())
        {
            soldier->pick(weapon);
        }
    }
}

int WeaponPickerUpdater::getPlayerID() const
{
    return playerID;
}

void WeaponPickerUpdater::clear()
{
    playerID = 0;
    pickFrom = pickTo = btVector3(0, 0, 0);

    weapon = nullptr;
}

WeaponPickerUpdater::~WeaponPickerUpdater()
{
    delete rayTracer;
}
