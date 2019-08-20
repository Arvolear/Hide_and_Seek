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

    localTransform = model = mat4(1.0);

    animation = nullptr;
}

void PlayerDataCollector::setPlayerID(int playerID)
{
    this->playerID = playerID;
}

void PlayerDataCollector::collect(Player* player)
{
    if (player->getGameObject())
    {
        localTransform = player->getGameObject()->getLocalTransform();
        model = player->getGameObject()->getPhysicsObjectTransform();
        animation = player->getGameObject()->getActiveAnimation();
    }
}

string PlayerDataCollector::getData() const
{
    XMLDocument playerDataCollectorDoc;

    /* root */
    XMLNode* root = playerDataCollectorDoc.NewElement("PlayerDataFile");

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
            string str;
            str += char('a' + (i * 4 + j));

            XMLElement* cellElem = playerDataCollectorDoc.NewElement(str.data());
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
            string str;
            str += char('a' + (i * 4 + j));
            
            XMLElement* cellElem = playerDataCollectorDoc.NewElement(str.data());
            cellElem->SetText(model[i][j]);

            modelElem->InsertEndChild(cellElem);
        }
    }

    root->InsertEndChild(modelElem);

    if (animation)
    {
        /* animation */
        XMLElement* animationElem = playerDataCollectorDoc.NewElement("animation");

        /* name */
        XMLElement* nameElem = playerDataCollectorDoc.NewElement("name");
        nameElem->SetText(animation->getName().c_str());

        animationElem->InsertEndChild(nameElem);

        /* animId */
        XMLElement* animIdElem = playerDataCollectorDoc.NewElement("animid");
        animIdElem->SetText(animation->getAnimId());

        animationElem->InsertEndChild(animIdElem);

        /* framesRange */
        XMLElement* framesRangeElem = playerDataCollectorDoc.NewElement("framesrange");
        framesRangeElem->SetAttribute("start", animation->getFramesRange().x);
        framesRangeElem->SetAttribute("end", animation->getFramesRange().y);

        animationElem->InsertEndChild(framesRangeElem);

        /* curFrame */
        XMLElement* curFrameElem = playerDataCollectorDoc.NewElement("curframe");
        curFrameElem->SetText(animation->getCurFrame());

        animationElem->InsertEndChild(curFrameElem);

        /* speed */
        XMLElement* speedElem = playerDataCollectorDoc.NewElement("speed");
        speedElem->SetText(animation->getSpeed());

        animationElem->InsertEndChild(speedElem);

        /* loop */
        XMLElement* loopElem = playerDataCollectorDoc.NewElement("loop");
        loopElem->SetText(animation->getLoop());

        animationElem->InsertEndChild(loopElem);

        root->InsertEndChild(animationElem);
    }

    /* printer */
    XMLPrinter playerDataCollectorPrinter;
    playerDataCollectorDoc.Print(&playerDataCollectorPrinter);

    string res("header\n");
    res += playerDataCollectorPrinter.CStr();
    res += "footer\n";

    return res;
}

PlayerDataCollector::~PlayerDataCollector() {}
