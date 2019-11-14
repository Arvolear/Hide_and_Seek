#include "../global/globaluse.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "weaponpickercollector.hpp"

WeaponPickerCollector::WeaponPickerCollector(int clients)
{
    playerID = 0;
    last.resize(clients, "");
}

void WeaponPickerCollector::collect(Player* player)
{
    Soldier* soldier = dynamic_cast < Soldier* >(player);

    if (!soldier)
    {
        throw(runtime_error("ERROR::WeaponPickerCollector::collect() player is not a soldier"));
    }

    playerID = soldier->getPhysicsObject()->getOwnerID();

    deque < Weapon* > new_weapons = soldier->getNewWeapons();

    for (int i = (int)new_weapons.size() - 1; i >= 0; i--)
    {
        names.push_back(new_weapons[i]->getName());
    }

    soldier->newToWeapons();
}

string WeaponPickerCollector::getData(int client) const
{   
    if (names.empty())
    {
        return "";
    }

    XMLDocument weaponPickerCollectorDoc;

    /* root */
    XMLNode* root = weaponPickerCollectorDoc.NewElement("Pick");
    weaponPickerCollectorDoc.InsertFirstChild(root);
    
    /* playerID */
    XMLElement* playerIDElem = weaponPickerCollectorDoc.NewElement("id");
    playerIDElem->SetText(playerID);

    root->InsertEndChild(playerIDElem);

    /* weapons */
    XMLElement* weaponsElem = weaponPickerCollectorDoc.NewElement("wpns");

    for (size_t i = 0; i < names.size(); i++)
    {
        XMLElement* nameElem = weaponPickerCollectorDoc.NewElement("name");
        nameElem->SetText(names[i].data());

        weaponsElem->InsertEndChild(nameElem);
    }

    root->InsertEndChild(weaponsElem);

    /* printer */
    XMLPrinter weaponPickerCollectorPrinter;
    weaponPickerCollectorDoc.Print(&weaponPickerCollectorPrinter);

    string res = weaponPickerCollectorPrinter.CStr();

    if (res == last[client])
    {
        return "";
    }
    else
    {
        last[client] = res;
    }

    /* timestamp */
    XMLElement* timeElem = weaponPickerCollectorDoc.NewElement("time");
    timeElem->SetAttribute("time", global.getTime());
    weaponPickerCollectorDoc.InsertFirstChild(timeElem);
    
    weaponPickerCollectorPrinter.ClearBuffer();
    weaponPickerCollectorDoc.Print(&weaponPickerCollectorPrinter);

    res = "";

    res += "BEG\n";
    res += weaponPickerCollectorPrinter.CStr();
    res += "END";

    return res;
}

void WeaponPickerCollector::clear()
{
    names.clear();
}

void WeaponPickerCollector::clearLast(int client)
{
    last[client] = "";
}

void WeaponPickerCollector::clearAllLast()
{
    for (size_t i = 0; i < last.size(); i++)
    {
        last[i] = "";
    }
}

WeaponPickerCollector::~WeaponPickerCollector() {}
