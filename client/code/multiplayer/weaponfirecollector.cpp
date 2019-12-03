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

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "weaponfirecollector.hpp"

WeaponFireCollector::WeaponFireCollector()
{
    playerID = 0;
}

void WeaponFireCollector::setPlayerID(int playerID)
{
    this->playerID = playerID;
}

void WeaponFireCollector::collect(Player* player)
{
    Soldier* soldier = dynamic_cast < Soldier* >(player);

    if (!soldier)
    {
        throw(runtime_error("ERROR::WeaponFireCollector::collect() player is not a soldier"));
    }

    fireInfo = soldier->getFire();
    reloadInfo = soldier->getReload();
}

string WeaponFireCollector::getData() const
{
    if (fireInfo.empty() && reloadInfo.empty())
    {
        return "";
    }

    XMLDocument weaponFireDoc;

    /* root */
    XMLNode* root = weaponFireDoc.NewElement("Fire");

    weaponFireDoc.InsertFirstChild(root);

    /* playerID */
    XMLElement* playerIDElem = weaponFireDoc.NewElement("id");
    playerIDElem->SetText(playerID);

    root->InsertEndChild(playerIDElem);

    for (auto& i : fireInfo)
    {     
        XMLElement* weaponElem = weaponFireDoc.NewElement("wpn");

        XMLElement* nameElem = weaponFireDoc.NewElement("name");
        nameElem->SetText(i.first.data());
        
        weaponElem->InsertEndChild(nameElem);

        auto it = reloadInfo.find(i.first);

        if (it != reloadInfo.end())
        {
            if (it->second)
            {
                XMLElement* reloadElem = weaponFireDoc.NewElement("rld");
                reloadElem->SetText("1");

                weaponElem->InsertEndChild(reloadElem);
            }

            reloadInfo.erase(i.first);
        }
        
        for (auto& j : i.second)
        {
            XMLElement* bulletElem = weaponFireDoc.NewElement("bllt");

            /* pickFrom */
            XMLElement* fromElem = weaponFireDoc.NewElement("frm");
            fromElem->SetAttribute("x", global.cutFloat(j.first.x, 4));
            fromElem->SetAttribute("y", global.cutFloat(j.first.y, 4));
            fromElem->SetAttribute("z", global.cutFloat(j.first.z, 4));

            bulletElem->InsertEndChild(fromElem);

            /* pickTo */
            XMLElement* toElem = weaponFireDoc.NewElement("to");
            toElem->SetAttribute("x", global.cutFloat(j.second.x, 4));
            toElem->SetAttribute("y", global.cutFloat(j.second.y, 4));
            toElem->SetAttribute("z", global.cutFloat(j.second.z, 4)); 
            
            bulletElem->InsertEndChild(toElem);

            weaponElem->InsertEndChild(bulletElem);
        }

        root->InsertEndChild(weaponElem);
    }
    
    for (auto& i : reloadInfo)
    {     
        XMLElement* weaponElem = weaponFireDoc.NewElement("wpn");

        XMLElement* nameElem = weaponFireDoc.NewElement("name");
        nameElem->SetText(i.first.data());

        weaponElem->InsertEndChild(nameElem);

        if (i.second)
        {
            XMLElement* reloadElem = weaponFireDoc.NewElement("rld");
            reloadElem->SetText("1");

            weaponElem->InsertEndChild(reloadElem);
        }
        
        root->InsertEndChild(weaponElem);
    }

    /* printer */
    XMLPrinter weaponFirePrinter;
    weaponFireDoc.Print(&weaponFirePrinter);

    string res("BEG\n");
    res += weaponFirePrinter.CStr();
    res += "END\n";

    return res;    
}

void WeaponFireCollector::clear()
{
    fireInfo.clear();
    reloadInfo.clear();
}

WeaponFireCollector::~WeaponFireCollector() {}
