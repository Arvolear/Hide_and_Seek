#include "../shader/shader.hpp"

#include "../global/convert.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../debug/debugsphere.hpp"
#include "../debug/debugdrawer.hpp"

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

GameObjectDataUpdater::GameObjectDataUpdater()
{
    name = "";
    model = mat4(1.0);
}

void GameObjectDataUpdater::collect(string info)
{
    XMLDocument gameObjectDataUpdaterDoc;
    
    gameObjectDataUpdaterDoc.Parse(info.data());

    /* root */
    XMLNode* root = gameObjectDataUpdaterDoc.FirstChildElement("Obj");

    if (!root)
    {
        gameObjectDataUpdaterDoc.PrintError();
        cout << info << endl << endl;
        throw runtime_error("ERROR::GameObjectDataUpdater::collect() failed to load XML");
    }

    /* name */
    XMLElement* nameElem = root->FirstChildElement("name");
    
    if (nameElem)
    {
        name = nameElem->GetText();
    }

    /* model */
    XMLElement* modelElem = root->FirstChildElement("mdl");

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
}

void GameObjectDataUpdater::updateData(GameObject* gameObject)
{
    gameObject->setPhysicsObjectTransform(model);
}

string GameObjectDataUpdater::getName() const
{
    return name;
}

GameObjectDataUpdater::~GameObjectDataUpdater() {}
