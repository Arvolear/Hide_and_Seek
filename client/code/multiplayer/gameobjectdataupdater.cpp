#include "../shader/shader.hpp"

#include "../global/globaluse.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

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

#include "gameobjectdataupdater.hpp"

GameObjectDataUpdater::GameObjectDataUpdater() {}

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
        string name = "";
        mat4 model = mat4(1.0);

        /* name */
        XMLElement* nameElem = objElem->FirstChildElement("name");

        if (nameElem)
        {
            name = nameElem->GetText();
        }

        /* model */
        XMLElement* modelElem = objElem->FirstChildElement("mdl");

        if (modelElem)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    string str;
                    str = char('a' + (i * 4 + j));

                    modelElem->QueryFloatAttribute(str.data(), &model[i][j]);
                }
            }
        }

        names.push_back(name);
        models.push_back(model);
        
        objElem = objElem->NextSiblingElement();
    }
}

void GameObjectDataUpdater::updateData(GameObject* gameObject, bool interpolation)
{
    gameObject->setPhysicsObjectTransform(models[0], interpolation);
}

void GameObjectDataUpdater::updateData(map < string, GameObject* > gameObjects, bool interpolation)
{
    for (size_t i = 0; i < names.size(); i++)
    {
        if (gameObjects.find(names[i]) != gameObjects.end())
        {
            gameObjects.find(names[i])->second->setPhysicsObjectTransform(models[i], interpolation); 
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
    names.clear();
    models.clear();
}

GameObjectDataUpdater::~GameObjectDataUpdater() {}
