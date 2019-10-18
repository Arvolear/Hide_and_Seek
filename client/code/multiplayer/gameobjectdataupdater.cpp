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
#include "gameobjectdataupdater.hpp"

GameObjectDataUpdater::GameObjectDataUpdater() 
{
    objParser = new PhysicsObjectDataParser();
}

void GameObjectDataUpdater::collect(string info)
{
    XMLDocument gameObjectDataUpdaterDoc;
    
    gameObjectDataUpdaterDoc.Parse(info.data());

    /* root */
    XMLNode* root = gameObjectDataUpdaterDoc.FirstChildElement("Objs");
    
    if (!root)
    {
        gameObjectDataUpdaterDoc.PrintError();
        cout << info << endl << endl;
        throw runtime_error("ERROR::GameObjectDataUpdater::collect() failed to load XML");
    }

    XMLElement* objElem = root->FirstChildElement("obj");

    while (objElem)
    {
        objParser->parse(objElem);

        objElem = objElem->NextSiblingElement();
    }

    names = objParser->getNames();
}

void GameObjectDataUpdater::updateData(GameObject* gameObject, bool interpolation)
{
    if (names.empty())
    {
        return;
    }

    objParser->updatePhysicsObject(gameObject, interpolation);
}

void GameObjectDataUpdater::updateData(map < string, GameObject* > gameObjects, bool interpolation)
{
    for (size_t i = 0; i < names.size(); i++)
    {
        auto it = gameObjects.find(names[i]);

        if (it != gameObjects.end())
        {
            objParser->updatePhysicsObject(it->second, interpolation);
        }
    }
}

string GameObjectDataUpdater::getName(int index) const
{
    return names[index];
}

vector < string > GameObjectDataUpdater::getNames() const
{
    return names;
}

void GameObjectDataUpdater::clear()
{
    objParser->clear();
    names.clear();
}

GameObjectDataUpdater::~GameObjectDataUpdater() 
{
    delete objParser;
}
