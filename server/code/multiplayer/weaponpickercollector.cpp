#include "../global/convert.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "weaponpickercollector.hpp"

WeaponPickerCollector::WeaponPickerCollector()
{
    playerID = 0;
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

string WeaponPickerCollector::getData() const
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

    string res("BEG\n");
    res += weaponPickerCollectorPrinter.CStr();
    res += "END";

    return res;
}

void WeaponPickerCollector::clear()
{
    names.clear();
}

WeaponPickerCollector::~WeaponPickerCollector() {}
