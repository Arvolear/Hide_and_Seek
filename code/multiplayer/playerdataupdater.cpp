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

#include "playerdataupdater.hpp"
        
PlayerDataUpdater::PlayerDataUpdater()
{
    playerID = 0;

    localTransform = model = mat4(1.0);

    animation = nullptr;
}

void PlayerDataUpdater::collect(string info)
{
    //cout << info << endl << endl << endl;

    XMLDocument playerDataUpdaterDoc;
    
    playerDataUpdaterDoc.Parse(info.c_str());
    //playerDataUpdaterDoc.PrintError();

    /* root */
    XMLNode* root = playerDataUpdaterDoc.FirstChildElement("PlayerDataFile");

    if (!root)
    {
        throw runtime_error("ERROR::PlayerDataUpdater::collect() failed to load XML");
    }

    /* playerID */
    XMLElement* playerIDElem = root->FirstChildElement("playerID");
    
    if (playerIDElem)
    {
        playerIDElem->QueryIntAttribute("playerID", &playerID);
    }

    /* localTransform */
    XMLElement* localTransformElem = root->FirstChildElement("localTransform");

    if (localTransformElem)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                string str;
                str += char('a' + (i * 4 + j));

                XMLElement* cellElem = localTransformElem->FirstChildElement(str.data());

                cellElem->QueryFloatText(&localTransform[i][j]);
            }
        }
    }      
    
    /* model */
    XMLElement* modelElem = root->FirstChildElement("model");

    if (modelElem)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                string str;
                str += char('a' + (i * 4 + j));

                XMLElement* cellElem = modelElem->FirstChildElement(str.data());

                cellElem->QueryFloatText(&model[i][j]);
            }
        }
    }

    /* up */
    XMLElement* upElem = root->FirstChildElement("up");
    upElem->QueryFloatAttribute("x", &up.x);
    upElem->QueryFloatAttribute("y", &up.y);
    upElem->QueryFloatAttribute("z", &up.z);
    
    /* forward */
    XMLElement* forwardElem = root->FirstChildElement("forward");
    forwardElem->QueryFloatAttribute("x", &forward.x);
    forwardElem->QueryFloatAttribute("y", &forward.y);
    forwardElem->QueryFloatAttribute("z", &forward.z); 
    
    /* moveDirection */
    XMLElement* moveDirectionElem = root->FirstChildElement("moveDirection");
    moveDirectionElem->QueryFloatAttribute("x", &moveDirection.x);
    moveDirectionElem->QueryFloatAttribute("y", &moveDirection.y);
    moveDirectionElem->QueryFloatAttribute("z", &moveDirection.z); 
    
    /* modelForward */
    XMLElement* modelForwardElem = root->FirstChildElement("modelForward");
    modelForwardElem->QueryFloatAttribute("x", &modelForward.x);
    modelForwardElem->QueryFloatAttribute("y", &modelForward.y);
    modelForwardElem->QueryFloatAttribute("z", &modelForward.z);  
    
    /* animation */
    XMLElement* animationElem = root->FirstChildElement("animation");

    if (animationElem)
    {
        XMLElement* nameElem = animationElem->FirstChildElement("name");
        const char* name = nameElem->GetText();

        animation = new Animation(name);

        XMLElement* animIdElem = animationElem->FirstChildElement("animid");
        int animId;
        animIdElem->QueryIntText(&animId);
        animation->setAnimId(animId);
        
        XMLElement* framesRangeElem = animationElem->FirstChildElement("framesrange");

        int x, y;
        framesRangeElem->QueryIntAttribute("start", &x);
        framesRangeElem->QueryIntAttribute("end", &y);

        animation->setFramesRange(vec2(x, y));

        XMLElement* curFrameElem = animationElem->FirstChildElement("curframe");
        int curFrame;
        curFrameElem->QueryIntText(&curFrame);
        animation->fromFrame(curFrame);
        
        XMLElement* speedElem = animationElem->FirstChildElement("speed");
        int speed;
        speedElem->QueryIntText(&speed);
        animation->setSpeed(speed);
        
        XMLElement* loopElem = animationElem->FirstChildElement("loop");
        bool loop;
        loopElem->QueryBoolText(&loop);
        animation->setLoop(loop);
    }
}

void PlayerDataUpdater::updateData(Player* player)
{
    player->getGameObject()->setLocalTransform(localTransform);
    player->getGameObject()->setPhysicsObjectTransform(model);

    player->setUp(up);
    player->setForward(forward);
    player->setMoveDirection(moveDirection);
    player->setModelForward(modelForward);

    if (animation)
    {
        if (player->getGameObject()->getActiveAnimation() && \
            player->getGameObject()->getActiveAnimation()->getName() == animation->getName())
        {
            return;
        }

        //player->getGameObject()->stopAnimation();
        //player->getGameObject()->removeAnimation(animation->getName());
        //player->getGameObject()->addAnimation(animation);
        player->getGameObject()->playAnimation(animation->getName());
    }
}

int PlayerDataUpdater::getPlayerID() const
{
    return playerID;
}

PlayerDataUpdater::~PlayerDataUpdater() {}
