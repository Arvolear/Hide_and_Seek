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

#include "physicsobjectdataparser.hpp"
#include "weapondataupdater.hpp"

WeaponDataUpdater::WeaponDataUpdater() 
{
    objParser = new PhysicsObjectDataParser();
}

void WeaponDataUpdater::collect(string info)
{
    XMLDocument gameObjectDataUpdaterDoc;
    
    gameObjectDataUpdaterDoc.Parse(info.data());

    /* root */
    XMLNode* root = gameObjectDataUpdaterDoc.FirstChildElement("Weapons");
    
    if (!root)
    {
        gameObjectDataUpdaterDoc.PrintError();
        cout << info << endl << endl;
        throw runtime_error("ERROR::WeaponDataUpdater::collect() failed to load XML");
    }

    XMLElement* weaponElem = root->FirstChildElement("weapon");

    while (weaponElem)
    {
        const char* name = nullptr;
        weaponElem->QueryStringAttribute("name", &name);

        objParser->parse(weaponElem);

        /* info */
        XMLElement* infoElem = weaponElem->FirstChildElement("info");

        if (infoElem)
        {
            XMLElement* storageElem = infoElem->FirstChildElement("storage");

            if (storageElem)
            {
                int storage;
                storageElem->QueryIntAttribute("amount", &storage);

                storages.insert({name, storage});
            }
            
            XMLElement* sizeElem = infoElem->FirstChildElement("size");

            if (sizeElem)
            {
                int size;
                sizeElem->QueryIntAttribute("size", &size);

                sizes.insert({name, size});
            }
            
            XMLElement* magazineElem = infoElem->FirstChildElement("magazine");

            if (magazineElem)
            {
                int magazine;
                magazineElem->QueryIntAttribute("amount", &magazine);

                magazines.insert({name, magazine});
            }
            
            XMLElement* speedElem = infoElem->FirstChildElement("speed");

            if (speedElem)
            {
                float speed;
                speedElem->QueryFloatAttribute("speed", &speed);

                speeds.insert({name, speed});
            }
            
            XMLElement* powerElem = infoElem->FirstChildElement("power");

            if (powerElem)
            {
                int power;
                powerElem->QueryIntAttribute("power", &power);

                powers.insert({name, power});
            } 
        }

        weaponElem = weaponElem->NextSiblingElement();
    }

    names = objParser->getNames();
}

void WeaponDataUpdater::updateData(map < string, GameObject* > gameObjects, bool interpolation)
{
    for (size_t i = 0; i < names.size(); i++)
    {
        auto it = gameObjects.find(names[i]);

        if (it != gameObjects.end())
        {
            Weapon* weapon = dynamic_cast < Weapon* >(it->second);

            if (weapon)
            {
                objParser->updatePhysicsObject(it->second, interpolation);

                auto storIt = storages.find(names[i]);
                auto sizeIt = sizes.find(names[i]);
                auto magazIt = magazines.find(names[i]);
                auto speedIt = speeds.find(names[i]);
                auto powerIt = powers.find(names[i]);

                if (storIt != storages.end())
                {
                    weapon->setStorageBullets(storIt->second);
                }
                
                if (sizeIt != sizes.end())
                {
                    weapon->setMagazineSize(sizeIt->second);
                }
                
                if (magazIt != magazines.end())
                {
                    weapon->setMagazineBullets(magazIt->second);
                }
                
                if (speedIt != speeds.end())
                {
                    weapon->setShotSpeed(speedIt->second);
                }
                
                if (powerIt != powers.end())
                {
                    weapon->setShotPower(powerIt->second);
                }
            }
            else
            {
                throw(runtime_error("ERROR::WTF? Rifle->Weapon"));
            }
        }
    }
}

vector < string > WeaponDataUpdater::getNames() const
{
    return names;
}

void WeaponDataUpdater::clear()
{
    objParser->clear();
    names.clear();

    storages.clear();
    sizes.clear();
    magazines.clear();
    speeds.clear();
    powers.clear();
}

WeaponDataUpdater::~WeaponDataUpdater() 
{
    delete objParser;
}
