#include "../global/globaluse.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "weapondatacollector.hpp"
        
WeaponDataCollector::WeaponDataCollector() {}

void WeaponDataCollector::collect(map < string, PhysicsObject* > weapons)
{
    for (auto& i: weapons)
    {
        Weapon* weapon = dynamic_cast < Weapon* >(i.second);

        if (weapon)
        {        
            pos.insert({i.first, weapon->getTransform()});
            storages.insert({i.first, weapon->getStorageBullets()});
            sizes.insert({i.first, weapon->getMagazineSize()});
            magazines.insert({i.first, weapon->getMagazineBullets()});
            speeds.insert({i.first, weapon->getShotSpeed()});
            powers.insert({i.first, weapon->getShotPower()});
        }
    }
}

string WeaponDataCollector::getMergedData(string fileName, bool raw) const
{
    XMLDocument weaponDataCollectorDoc;

    weaponDataCollectorDoc.LoadFile(fileName.c_str());

    XMLNode* root = weaponDataCollectorDoc.FirstChildElement("Weapons");

    if (!root)
    {
        throw runtime_error("ERROR::WeaponDataCollector::getMergedData() failed to load XML");
    }
    
    /* timestamp */
    XMLElement* timeElem = weaponDataCollectorDoc.NewElement("time");
    timeElem->SetAttribute("time", global.getTime());
    //root->InsertFirstChild(timeElem);

    XMLElement* weaponElem = root->FirstChildElement("weapon");

    while (weaponElem)
    {
        const char* name = nullptr;
        weaponElem->QueryStringAttribute("name", &name);

        XMLElement* positionElem = weaponElem->FirstChildElement("pos");
        
        if (positionElem)
        {
            weaponElem->DeleteChild(positionElem);
        }

        XMLElement* rotationElem = weaponElem->FirstChildElement("rot");

        while (rotationElem && !strcmp(rotationElem->Value(), "rot"))
        {
            weaponElem->DeleteChild(rotationElem);
            rotationElem = rotationElem->NextSiblingElement();
        }

        /* model */
        XMLElement* modelElem = weaponDataCollectorDoc.NewElement("mdl");

        btScalar* model = pos[name];

        if (model)
        {
            for (int j = 0; j < 16; j++)
            {
                string str;
                str = char('a' + j);

                modelElem->SetAttribute(str.data(), global.cutFloat(model[j], 4));
            }

            weaponElem->InsertEndChild(modelElem);
        }

        XMLElement* infoElem = weaponElem->FirstChildElement("info");

        if (infoElem)
        {
            XMLElement* storageElem = infoElem->FirstChildElement("storage");

            if (storageElem)
            {
                storageElem->SetAttribute("amount", storages[name]);
            }
            
            XMLElement* sizeElem = infoElem->FirstChildElement("size");

            if (sizeElem)
            {
                sizeElem->SetAttribute("size", sizes[name]);
            }
            
            XMLElement* magazineElem = infoElem->FirstChildElement("magazine");

            if (magazineElem)
            {
                magazineElem->SetAttribute("amount", magazines[name]);
            }
            
            XMLElement* speedElem = infoElem->FirstChildElement("speed");

            if (speedElem)
            {
                speedElem->SetAttribute("speed", speeds[name]);
            }
            
            XMLElement* powerElem = infoElem->FirstChildElement("power");

            if (powerElem)
            {
                powerElem->SetAttribute("power", powers[name]);
            }
        }

        weaponElem = weaponElem->NextSiblingElement();
    }

    /* printer */
    XMLPrinter weaponDataCollectorPrinter;
    weaponDataCollectorDoc.Print(&weaponDataCollectorPrinter);

    string res;

    if (!raw)
    {
        res = "BEG\n";
        res += weaponDataCollectorPrinter.CStr();
        res += "END";
    }
    else
    {
        res = weaponDataCollectorPrinter.CStr();
    }

    return res;
}

void WeaponDataCollector::clear()
{
    for (auto& i: pos)
    {
        delete[] i.second;
    }

    pos.clear();
    storages.clear();
    sizes.clear();
    magazines.clear();
    speeds.clear();
    powers.clear();
}

WeaponDataCollector::~WeaponDataCollector() 
{
    clear();
}
