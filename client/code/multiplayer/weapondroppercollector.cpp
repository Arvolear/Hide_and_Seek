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

#include "weapondroppercollector.hpp"

WeaponDropperCollector::WeaponDropperCollector() 
{
    playerID = 0;
    dropTo = false;
    model = mat4(1.0);
}

void WeaponDropperCollector::setPlayerID(int playerID)
{
    this->playerID = playerID;
}

void WeaponDropperCollector::collect(Player* player)
{
    Soldier* soldier = dynamic_cast < Soldier* >(player);

    if (!soldier)
    {
        throw(runtime_error("ERROR::WeaponDropperCollector::collect() player is not a soldier"));
    }

    dropTo = soldier->isDrop();
    
    if (!dropTo)
    {
        return;
    }

    deque < Weapon* > weapons = soldier->getWeapons();

    if (!weapons.empty() && weapons[0])
    {
        model = weapons[0]->getPhysicsObjectTransform();
    }

    soldier->clearDropData();
}

string WeaponDropperCollector::getData() const
{
    if (!dropTo)
    {
        return "";
    }

    XMLDocument weaponDropperDoc;

    /* root */
    XMLNode* root = weaponDropperDoc.NewElement("Drop");

    weaponDropperDoc.InsertFirstChild(root);

    /* playerID */
    XMLElement* playerIDElem = weaponDropperDoc.NewElement("id");
    playerIDElem->SetText(playerID);

    root->InsertEndChild(playerIDElem);

    /* model */
    XMLElement* modelElem = weaponDropperDoc.NewElement("mdl");

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            string str;
            str = char('a' + (i * 4 + j));

            modelElem->SetAttribute(str.data(), global.cutFloat(model[i][j], 4));
        }
    }

    root->InsertEndChild(modelElem);
    
    /* printer */
    XMLPrinter weaponDropperPrinter;
    weaponDropperDoc.Print(&weaponDropperPrinter);

    string res("BEG\n");
    res += weaponDropperPrinter.CStr();
    res += "END\n";

    return res;    
}

void WeaponDropperCollector::clear()
{
    dropTo = false;
    model = mat4(1.0);
}

WeaponDropperCollector::~WeaponDropperCollector() {}
