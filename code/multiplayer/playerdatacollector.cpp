#include "../shader/shader.hpp"

#include "../global/convert.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

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

#include "../world/raytracer.hpp"

#include "../player/player.hpp"

#include "playerdatacollector.hpp"
        
PlayerDataCollector::PlayerDataCollector()
{
    playerID = 0;

    meshWithBones = false;
    localTransform = model = mat4(1.0);
}

void PlayerDataCollector::setPlayerID(int playerID)
{
    this->playerID = playerID;
}

void PlayerDataCollector::collect(Player* player)
{
    localTransform = player->getGameObject()->getLocalTransform();
    model = player->getGameObject()->getPhysicsObjectTransform();
    
    meshWithBones = player->getGameObject()->getSkeleton()->isMeshWithBones();
    bones = player->getGameObject()->getSkeleton()->getBonesMatrices();
}

string PlayerDataCollector::getData() const
{
    XMLDocument playerDataCollectorDoc;

    /* root */
    XMLNode* root = playerDataCollectorDoc.NewElement("PlayerDataCollectorFile");

    playerDataCollectorDoc.InsertFirstChild(root);

    /* playerID */
    XMLElement* playerIDElem = playerDataCollectorDoc.NewElement("playerID");
    playerIDElem->SetAttribute("playerID", playerID);

    root->InsertEndChild(playerIDElem);

    /* localTransform */
    XMLElement* localTransformElem = playerDataCollectorDoc.NewElement("localTransform");

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            XMLElement* cellElem = playerDataCollectorDoc.NewElement((to_string(i) + ";" + to_string(j)).c_str());
            cellElem->SetText(localTransform[i][j]);

            localTransformElem->InsertEndChild(cellElem);
        }
    }

    root->InsertEndChild(localTransformElem);

    /* model */
    XMLElement* modelElem = playerDataCollectorDoc.NewElement("model");

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            XMLElement* cellElem = playerDataCollectorDoc.NewElement((to_string(i) + ";" + to_string(j)).c_str());
            cellElem->SetText(model[i][j]);

            modelElem->InsertEndChild(cellElem);
        }
    }

    root->InsertEndChild(modelElem);

    /* bones */
    XMLElement* bonesElem = playerDataCollectorDoc.NewElement("bones");
    bonesElem->SetAttribute("amount", (unsigned int)bones.size());

    for (size_t k = 0; k < bones.size(); k++)
    {
        XMLElement* boneElem = playerDataCollectorDoc.NewElement(("bone" + to_string(k)).c_str());

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                XMLElement* cellElem = playerDataCollectorDoc.NewElement((to_string(i) + ";" + to_string(j)).c_str());
                cellElem->SetText(bones[k][i][j]);

                boneElem->InsertEndChild(cellElem);
            }
        }

        bonesElem->InsertEndChild(boneElem);
    }

    root->InsertEndChild(bonesElem);

    XMLPrinter playerDataCollectorPrinter;
    playerDataCollectorDoc.Print(&playerDataCollectorPrinter);

    string res = playerDataCollectorPrinter.CStr();

    return res;
}

PlayerDataCollector::~PlayerDataCollector() {}
