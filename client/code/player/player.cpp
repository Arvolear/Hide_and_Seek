#include "../shader/shader.hpp"

#include "../global/convert.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "camera.hpp"

#include "../debug/debugsphere.hpp"

#include "../game_object/animation.hpp"
#include "../game_object/mesh.hpp"
#include "../game_object/bone.hpp"
#include "../game_object/skeleton.hpp"
#include "../game_object/viewfrustum.hpp"
#include "../game_object/boundsphere.hpp"
#include "../game_object/modelloader.hpp"
#include "../game_object/gameobject.hpp"

#include "../world/raytracer.hpp"

#include "player.hpp"

Player::Player(Window* window, vec3 playerPos, vec3 cameraForward, float speed) : Camera(window, playerPos, cameraForward, speed)
{
    gameObject = nullptr;

    cameraOffset = modelOffset = vec3(0);
    modelForward = normalize(cross(Left, Up));

    headCenter = vec3(0);

    setActive(false);
}
        
Player::Player(Window* window, vec3 playerPos, vec3 cameraForward, GameObject* player, float speed, bool active) : Camera(window, playerPos, cameraForward, speed)
{
    this->gameObject = gameObject;
    
    cameraOffset = modelOffset = vec3(0);
    modelForward = normalize(cross(Left, Up));
    
    headCenter = vec3(0);
    
    setActive(active);
}

/*
dist > 2.2
*/

void Player::moveAction()
{
    unique_lock < mutex> lk(mtx);
    ready = false;

    move = "";
    vec3 playerForward = normalize(cross(Left, Up));

    if (window->isKeyPressed(GLFW_KEY_W))
    {
        if (gameObject)
        {
            moveDirection += playerForward;
            move += "W";
        }
        else
        {
            moveDirection += Forward;
        }
    }

    if (window->isKeyPressed(GLFW_KEY_S))
    {
        if (gameObject)
        {
            moveDirection -= playerForward;
            move += "S";
        }
        else
        {
            moveDirection -= Forward;
        }
    }

    if (window->isKeyPressed(GLFW_KEY_D))
    {
        moveDirection -= Left;
        move += "D";
    }

    if (window->isKeyPressed(GLFW_KEY_A))
    {
        moveDirection += Left;
        move += "A";
    }

    if (!gameObject)
    {
        if (window->isKeyPressed(GLFW_KEY_E))
        {
            moveDirection += Up;
        }

        if (window->isKeyPressed(GLFW_KEY_Q))
        {
            moveDirection -= Up;
        }
    }

    if (gameObject)
    {
        if (window->isKeyPressedOnce(GLFW_KEY_SPACE))
        {
            move += "J";
        }
    }

    ready = true;
    cv.notify_all();
}

void Player::calcCameraPosition()
{
    /* head center */
    setPosition(headCenter);

    Pos += normalize(cross(Left, Up)) * cameraOffset.x;
    Pos += Up * cameraOffset.y;
    Pos += Left * cameraOffset.z;
}

void Player::updateAnimation()
{
    if (!gameObject->getActiveAnimation() || (gameObject->getActiveAnimation()->getName() != "idle" && moveDirection == vec3(0)))
    {
        gameObject->playAnimation("idle");
    }
    else if (gameObject->getActiveAnimation() && gameObject->getActiveAnimation()->getName() != "run" && moveDirection != vec3(0))
    {
        gameObject->playAnimation("run");
    }
}

void Player::calcModelPosition()
{
    float angle = 0.0;
    
    if (moveDirection != vec3(0))
    {
        modelForward = normalize(modelForward);
        moveDirection = normalize(moveDirection);

        float cosRotAngle = dot(modelForward, moveDirection);

        if (cosRotAngle < -1)
        {
            cosRotAngle = -1;
        }
        else if (cosRotAngle > 1)
        {
            cosRotAngle = 1;
        }

        angle = toDegs(acos(cosRotAngle)); 

        vec3 diff = moveDirection - modelForward;

        if (dot(cross(Up, modelForward), diff) < 0)
        {
            angle *= -1;
        }
    
        modelForward = moveDirection;
    }

    //cout << angle << endl;

    gameObject->setLocalRotation(Up, angle);

    vec3 localPos = vec3(0);

    localPos += modelForward * modelOffset.x;
    localPos += Up * modelOffset.y;
    localPos += Left * modelOffset.z;

    gameObject->setLocalPosition(localPos, false);
}

void Player::moveGraphics()
{
    if (moveDirection != vec3(0))
    {
        moveDirection = normalize(moveDirection); // normalizing move direction for equal speeds
    }

    /* if physics object is applied */
    if (gameObject)
    {
        /* calc camera position */
        calcCameraPosition();

        /* rotate model */
        calcModelPosition();

        /* update animation */
        updateAnimation();
    }
    else
    {
        Pos += moveDirection * vec3(speed);
    }

    moveDirection = vec3(0);
}

void Player::setActive(bool active)
{
    this->active = active;

    if (gameObject)
    {
        gameObject->setVisible(!active);
    }
}

void Player::setGameObject(GameObject* gameObject)
{
    this->gameObject = gameObject;
}

void Player::setHeadCenter(vec3 headCenter)
{
    this->headCenter = headCenter;
}

void Player::setCameraOffset(vec3 cameraOffset)
{
    this->cameraOffset = cameraOffset;
}

void Player::setModelOffset(vec3 modelOffset)
{
    this->modelOffset = modelOffset;
}
        
void Player::setModelForward(vec3 modelForward)
{
    this->modelForward = modelForward;
}

void Player::removeGameObject()
{
    gameObject = nullptr;
}

bool Player::isActive() const
{
    return active;
}

void Player::update(bool events)
{
    if (events && active)
    {
        lookAction();
        moveAction();
    }

    moveGraphics();
}

GameObject* Player::getGameObject() const
{
    return gameObject;
}

Player::~Player() {}
