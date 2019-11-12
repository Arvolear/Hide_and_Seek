#include "../global/globaluse.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "weapondroppercollector.hpp"

WeaponDropperCollector::WeaponDropperCollector()
{
    playerID = 0;
}

void WeaponDropperCollector::collect(Player* player)
{
    Soldier* soldier = dynamic_cast < Soldier* >(player);

    if (!soldier)
    {
        throw(runtime_error("ERROR::WeaponDropperCollector::collect() player is not a soldier"));
    }

    playerID = soldier->getPhysicsObject()->getOwnerID();

    deque < Weapon* > old_weapons = soldier->getOldWeapons();

    for (size_t i = 0; i < old_weapons.size(); i++)
    {
        names.push_back(old_weapons[i]->getName());
    }

    soldier->oldToNothing();
}

string WeaponDropperCollector::getData() const
{   
    if (names.empty())
    {
        return "";
    }

    XMLDocument weaponDropperCollectorDoc;

    /* root */
    XMLNode* root = weaponDropperCollectorDoc.NewElement("Drop");
    weaponDropperCollectorDoc.InsertFirstChild(root);
    
    /* timestamp */
    XMLElement* timeElem = weaponDropperCollectorDoc.NewElement("time");
    timeElem->SetAttribute("time", global.getTime());
    //root->InsertFirstChild(timeElem);

    /* playerID */
    XMLElement* playerIDElem = weaponDropperCollectorDoc.NewElement("id");
    playerIDElem->SetText(playerID);

    root->InsertEndChild(playerIDElem);

    /* weapons */
    XMLElement* weaponsElem = weaponDropperCollectorDoc.NewElement("wpns");

    for (size_t i = 0; i < names.size(); i++)
    {
        XMLElement* nameElem = weaponDropperCollectorDoc.NewElement("name");
        nameElem->SetText(names[i].data());

        weaponsElem->InsertEndChild(nameElem);
    }

    root->InsertEndChild(weaponsElem);

    /* printer */
    XMLPrinter weaponDropperCollectorPrinter;
    weaponDropperCollectorDoc.Print(&weaponDropperCollectorPrinter);

    string res("BEG\n");
    res += weaponDropperCollectorPrinter.CStr();
    res += "END";

    return res;
}

void WeaponDropperCollector::clear()
{
    names.clear();
}

WeaponDropperCollector::~WeaponDropperCollector() {}
