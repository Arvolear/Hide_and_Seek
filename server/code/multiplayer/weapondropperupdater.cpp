#include "../global/convert.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "weapondropperupdater.hpp"

WeaponDropperUpdater::WeaponDropperUpdater()
{
    playerID = 0;
    model = new btScalar[16];
    memset(model, 0, sizeof(btScalar) * 16);
}

void WeaponDropperUpdater::collect(string info)
{
    XMLDocument weaponDropperUpdaterDoc;
    
    weaponDropperUpdaterDoc.Parse(info.c_str());

    /* root */
    XMLNode* root = weaponDropperUpdaterDoc.FirstChildElement("Drop");

    if (!root)
    {
        weaponDropperUpdaterDoc.PrintError();
        cout << info << endl << endl;
        throw runtime_error("ERROR::WeaponDropperUpdater::collect() failed to load XML");
    }

    /* playerID */
    XMLElement* playerIDElem = root->FirstChildElement("id");
    
    if (playerIDElem)
    {
        playerIDElem->QueryIntText(&playerID);
    }

    /* model */
    XMLElement* modelElem = root->FirstChildElement("mdl");

    for (int i = 0; i < 16; i++)
    {
        string str;
        str = char('a' + i);

        modelElem->QueryFloatAttribute(str.data(), &model[i]);
    }
}

void WeaponDropperUpdater::updateData(Player* player)
{
    Soldier* soldier = dynamic_cast < Soldier* >(player);

    if (!soldier)
    {
        throw(runtime_error("ERROR::WeaponDropperUPdater::updatedata() player is not a soldier"));
    }

    soldier->drop(model);
}

int WeaponDropperUpdater::getPlayerID() const
{
    return playerID;
}

void WeaponDropperUpdater::clear()
{
    playerID = 0;
    memset(model, 0, sizeof(btScalar) * 16);
}

WeaponDropperUpdater::~WeaponDropperUpdater()
{
    delete[] model;
}
