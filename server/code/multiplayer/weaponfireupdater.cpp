#include "../global/globaluse.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "weaponfireupdater.hpp"

WeaponFireUpdater::WeaponFireUpdater(World* world)
{
    playerID = 0;
    this->world = world;
    rayTracer = new RayTracer(world->getWorld());
}

void WeaponFireUpdater::collect(string info)
{
    XMLDocument weaponFireUpdaterDoc;
    
    weaponFireUpdaterDoc.Parse(info.c_str());

    /* root */
    XMLNode* root = weaponFireUpdaterDoc.FirstChildElement("Fire");

    if (!root)
    {
        weaponFireUpdaterDoc.PrintError();
        cout << info << endl << endl;
        throw runtime_error("ERROR::WeaponFireUpdater::collect() failed to load XML");
    }

    /* playerID */
    XMLElement* playerIDElem = root->FirstChildElement("id");
    
    if (playerIDElem)
    {
        playerIDElem->QueryIntText(&playerID);
    }

    XMLElement* weaponElem = root->FirstChildElement("wpn");
    
    while (weaponElem)
    {
        XMLElement* nameElem = weaponElem->FirstChildElement("name");
        const char* name = nameElem->GetText();

        vector < pair < btVector3, btVector3 > > bullets;
        
        XMLElement* reloadElem = weaponElem->FirstChildElement("rld");

        if (reloadElem)
        {
            const char* reload = reloadElem->GetText();
            
            if (!strcmp(reload, "1"))
            {
                reloadInfo.insert({name, true});
            }
        }

        XMLElement* bulletElem = weaponElem->FirstChildElement("bllt");

        while (bulletElem)
        {
            btVector3 from, to;

            /* pickFrom */
            XMLElement* fromElem = bulletElem->FirstChildElement("frm");

            if (fromElem)
            {
                float x, y, z;

                fromElem->QueryFloatAttribute("x", &x);
                fromElem->QueryFloatAttribute("y", &y);
                fromElem->QueryFloatAttribute("z", &z);

                from = btVector3(x, y, z);
            }

            /* pickTo */
            XMLElement* toElem = bulletElem->FirstChildElement("to");

            if (toElem)
            {
                float x, y, z;

                toElem->QueryFloatAttribute("x", &x);
                toElem->QueryFloatAttribute("y", &y);
                toElem->QueryFloatAttribute("z", &z);

                to = btVector3(x, y, z);
            }

            bullets.push_back({from, to});

            bulletElem = bulletElem->NextSiblingElement();
        }

        if (!bullets.empty())
        {
            fireInfo.insert({name, bullets});
        }

        weaponElem = weaponElem->NextSiblingElement();
    }
}

void WeaponFireUpdater::updateData(Player* player, map < string, PhysicsObject* > physicsObjects)
{
    for (auto &i : fireInfo)
    {
        auto it = physicsObjects.find(i.first);

        if (it == physicsObjects.end())
        {
            continue;
        }

        Weapon* WE = dynamic_cast < Weapon* >(it->second);

        if (!WE)
        {
            continue;
        }

        auto jt = reloadInfo.find(i.first);

        if (jt != reloadInfo.end())
        {
            WE->reload();

            reloadInfo.erase(i.first);
        }

        for (auto &j : i.second)
        { 
            /* no bullets */
            if (!WE->fire())
            {
                break;
            }

            unique_ptr < RayResult > result(rayTracer->rayCast(player->getPhysicsObject()->getRigidBody(), j.first, j.second, false));

            if (!result.get())
            {
                continue;
            }

            PhysicsObject* PO = static_cast < PhysicsObject* >(result->body->getUserPointer());
            
            //cout << PO->getName() << endl;

            if (!PO->getUserPointer())
            {
                continue;
            }

            Soldier* soldier = static_cast < Soldier* >(PO->getUserPointer());

            int dmg = WE->getShotPower();

            soldier->damage(dmg);
        }
    }
    
    for (auto &i : reloadInfo)
    {
        auto it = physicsObjects.find(i.first);

        if (it == physicsObjects.end())
        {
            continue;
        }

        Weapon* WE = dynamic_cast < Weapon* >(it->second);

        if (!WE)
        {
            continue;
        }

        WE->reload();
    }
}

int WeaponFireUpdater::getPlayerID() const
{
    return playerID;
}

void WeaponFireUpdater::clear()
{
    playerID = 0;

    fireInfo.clear();
    reloadInfo.clear();
}

WeaponFireUpdater::~WeaponFireUpdater()
{
    delete rayTracer;
}
