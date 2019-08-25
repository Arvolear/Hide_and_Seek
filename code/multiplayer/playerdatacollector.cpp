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

    up = moveDirection = forward = modelForward = vec3(1.0);

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
        up = player->getUp();
        forward = player->getForward();
        moveDirection = player->getPrevMoveDirection();
        modelForward = player->getModelForward();
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
    
    /* up */
    XMLElement* upElem = playerDataCollectorDoc.NewElement("up");
    upElem->SetAttribute("x", up.x);
    upElem->SetAttribute("y", up.y);
    upElem->SetAttribute("z", up.z);
    
    root->InsertEndChild(upElem);
    
    /* forward */
    XMLElement* forwardElem = playerDataCollectorDoc.NewElement("forward");
    forwardElem->SetAttribute("x", forward.x);
    forwardElem->SetAttribute("y", forward.y);
    forwardElem->SetAttribute("z", forward.z);
    
    root->InsertEndChild(forwardElem);
    
    /* moveDirection */
    XMLElement* moveDirectionElem = playerDataCollectorDoc.NewElement("moveDirection");
    moveDirectionElem->SetAttribute("x", moveDirection.x);
    moveDirectionElem->SetAttribute("y", moveDirection.y);
    moveDirectionElem->SetAttribute("z", moveDirection.z);
    
    root->InsertEndChild(moveDirectionElem);
    
    /* modelForward */
    XMLElement* modelForwardElem = playerDataCollectorDoc.NewElement("modelForward");
    modelForwardElem->SetAttribute("x", modelForward.x);
    modelForwardElem->SetAttribute("y", modelForward.y);
    modelForwardElem->SetAttribute("z", modelForward.z);
    
    root->InsertEndChild(modelForwardElem);

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
