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

#include "gameobjectdatacollector.hpp"
        
GameObjectDataCollector::GameObjectDataCollector()
{
    name = "";
    senderID = -1;
    model = mat4(1.0);
}

void GameObjectDataCollector::setSenderID(int senderID)
{
    this->senderID = senderID;
}

void GameObjectDataCollector::collect(GameObject* gameObject)
{
    if (gameObject)
    {
        model = gameObject->getPhysicsObjectTransform();
        name = gameObject->getName();
    }
}

string GameObjectDataCollector::getData() const
{
    XMLDocument gameObjectDataCollectorDoc;

    /* root */
    XMLNode* root = gameObjectDataCollectorDoc.NewElement("Obj");

    gameObjectDataCollectorDoc.InsertFirstChild(root);

    /* name */
    XMLElement* nameElem = gameObjectDataCollectorDoc.NewElement("name");
    nameElem->SetText(name.data());

    root->InsertEndChild(nameElem);
    
    /* senderID */
    XMLElement* senderIDElem = gameObjectDataCollectorDoc.NewElement("id");
    senderIDElem->SetText(senderID);

    root->InsertEndChild(senderIDElem);

    /* model */
    XMLElement* modelElem = gameObjectDataCollectorDoc.NewElement("mdl");

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            string str;
            str = char('a' + (i * 4 + j));

            modelElem->SetAttribute(str.data(), cutFloat(model[i][j], 4));
        }
    }

    root->InsertEndChild(modelElem);

    /* printer */
    XMLPrinter gameObjectDataCollectorPrinter;
    gameObjectDataCollectorDoc.Print(&gameObjectDataCollectorPrinter);

    string res("BEG\n");
    res += gameObjectDataCollectorPrinter.CStr();
    res += "END";

    return res;
}

void GameObjectDataCollector::clear()
{
    name = "";
    model = mat4(1.0);
}

GameObjectDataCollector::~GameObjectDataCollector() {}
