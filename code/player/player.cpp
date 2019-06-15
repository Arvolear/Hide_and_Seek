#include "../shader/shader.hpp"

#include "../global/convert.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "camera.hpp"

#include "../debug/debugsphere.hpp"

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

#include "player.hpp"

Player::Player(Window* window, vec3 playerPos, vec3 cameraForward, float speed) : Camera(window, playerPos, cameraForward, speed)
{
    rayTracer = nullptr;
    player = nullptr;

    jumpAllowed = true;
    speedLock = false;

    offset = vec2(0);
}
        
Player::Player(Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed) : Camera(window, playerPos, cameraForward, speed)
{
    rayTracer = tracer;
    this->player = player;
    
    jumpAllowed = true;
    speedLock = false;
    
    offset = vec2(0);
}

/*
dist > 2.2
*/

bool Player::isGroundStanding()
{
    if (!(player && player->getPhysicsObject() && rayTracer))
    {
        return false;
    }

    btVector3 from = player->getPhysicsObject()->getRigidBody()->getCenterOfMassPosition();
    btVector3 to = btVector3(0, -1, 0);

    RayResult* result = rayTracer->rayCast(from, to, false);

    if (!result)
    {
        return false;
    }

    float dist = (from - result->hitPoint).length();

    btVector3 Min;
    btVector3 Max;

    player->getPhysicsObject()->getRigidBody()->getAabb(Min, Max);

    float bottomDist = (from - btVector3(from.x(), Min.y(), from.z())).length();

    delete result;

    //cout << dist << " " << bottomDist << endl; 
    
    if (dist > bottomDist + 0.1)
    {
        return false;
    }

    return true;
}

void Player::jump()
{
    if (!(player && player->getPhysicsObject() && rayTracer && jumpAllowed))
    {
        return;
    }

    float power = 70;
    float loss = 0.5;

    //cout << "jump" << endl;

    player->getPhysicsObject()->getRigidBody()->setActivationState(ACTIVE_TAG);

    /* redusing the speed */
    btVector3 velocity = player->getPhysicsObject()->getRigidBody()->getLinearVelocity();
    velocity = btVector3(velocity.x() * loss, velocity.y(), velocity.z() * loss);
    player->getPhysicsObject()->getRigidBody()->setLinearVelocity(velocity);

    player->getPhysicsObject()->getRigidBody()->applyCentralImpulse(btVector3(0, 1, 0) * power);

    jumpAllowed = false;
}

void Player::moveAction()
{
    vec3 playerForward = normalize(cross(Left, Up));

    if (window->isKeyPressed(GLFW_KEY_W))
    {
        if (player && player->getPhysicsObject() && rayTracer)
        {
            moveDirection += playerForward;
        }
        else
        {
            moveDirection += Forward;
        }
    }

    if (window->isKeyPressed(GLFW_KEY_S))
    {
        if (player && player->getPhysicsObject() && rayTracer)
        {
            moveDirection -= playerForward;
        }
        else
        {
            moveDirection -= Forward;
        }
    }

    if (window->isKeyPressed(GLFW_KEY_D))
    {
        moveDirection -= Left;
    }

    if (window->isKeyPressed(GLFW_KEY_A))
    {
        moveDirection += Left;
    }

    if (!(player && player->getPhysicsObject() && rayTracer))
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

    if (player && player->getPhysicsObject() && rayTracer)
    {
        if (window->isKeyPressedOnce(GLFW_KEY_SPACE))
        {
            jump();
        }
    }
}

void Player::calcCameraPosition()
{
    btTransform globalTransform;
    btTransform headTransform;
    btVector3 globalCenter;
    btVector3 headCenter;
    btScalar headRadius;

    globalTransform = player->getPhysicsObject()->getRigidBody()->getCenterOfMassTransform();
    headTransform = player->getPhysicsObject()->getCompoundShape()->getChildTransform(0);
    player->getPhysicsObject()->getCompoundShape()->getChildShape(0)->getBoundingSphere(headCenter, headRadius);

    globalCenter = player->getPhysicsObject()->getRigidBody()->getCenterOfMassPosition();
    headCenter = globalTransform * headTransform * headCenter;
    setPosition(headCenter.x(), headCenter.y(), headCenter.z());

    /* new Up */
    Up = normalize(toVec3(headCenter) - toVec3(globalCenter));

    Pos += normalize(cross(Left, Up)) * offset.x;
    Pos += Up * offset.y;

    player->getPhysicsObject()->getRigidBody()->forceActivationState(ACTIVE_TAG);
}

void Player::moveGround()
{
    float speedFactor = 34;
    float friction = 0.6;

    /* push the body */
    player->getPhysicsObject()->getRigidBody()->applyCentralImpulse(btVector3(moveDirection.x, 0, moveDirection.z) * speed * speedFactor);

    btVector3 velocity = player->getPhysicsObject()->getRigidBody()->getLinearVelocity();

    /* disable sliding effect */
    if (velocity.length() < 0.1)
    {
        player->getPhysicsObject()->getRigidBody()->setActivationState(WANTS_DEACTIVATION);
    }

    /* friction */
    velocity = btVector3(velocity.x() * friction, velocity.y(), velocity.z() * friction);

    player->getPhysicsObject()->getRigidBody()->setLinearVelocity(velocity);

    jumpAllowed = true;
}

void Player::moveAir()
{
    float speedFactor = 2.2;

    jumpAllowed = false;

    /* push the body */
    player->getPhysicsObject()->getRigidBody()->applyCentralImpulse(btVector3(moveDirection.x, 0, moveDirection.z) * speed * speedFactor);

    btVector3 velocity = player->getPhysicsObject()->getRigidBody()->getLinearVelocity();

    /* enable sliding effect */
    if (abs(velocity.y()) < 0.1)
    {
        player->getPhysicsObject()->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    }
}

void Player::speedHackControl()
{
    float maxSpeed = 6;

    /* speed hack control */
    btVector3 velocity = player->getPhysicsObject()->getRigidBody()->getLinearVelocity();
    btVector3 XZVelocity = velocity;
    XZVelocity.setY(0);

    if (XZVelocity.length() > maxSpeed)
    {
        btVector3 XZNormVel = XZVelocity;
        XZNormVel.normalize();

        velocity -= (XZVelocity.length() - maxSpeed) * XZNormVel; 

        player->getPhysicsObject()->getRigidBody()->setLinearVelocity(velocity);
    }
}

void Player::movePhysics()
{
    if (moveDirection != vec3(0, 0, 0))
    {
        moveDirection = normalize(moveDirection); // normalizing move direction for equal speeds
    }

    /* if physics object is applied */
    if (player && player->getPhysicsObject() && rayTracer)
    {
        if (!player->getPhysicsObject()->getCompoundShape())
        {
            throw runtime_error("ERROR::player no compound shape"); 
        }

        if (player->getPhysicsObject()->getCompoundShape()->getShape()->getNumChildShapes() < 1)
        {
            throw runtime_error("ERROR::player compound shape < 1 children"); 
        }

        if (isGroundStanding()) // ground movement
        { 
            moveGround();
        }
        else // air movement
        {
            moveAir();
        }

        /* speed hack control */
        speedHackControl();
        
        /* calc camera position */
        calcCameraPosition();
    }
    else
    {
        Pos += moveDirection * vec3(speed);
    }

    moveDirection = vec3(0, 0, 0);
}

void Player::setRayTracer(RayTracer* tracer)
{
    this->rayTracer = tracer;
}

void Player::setGameObject(GameObject* player)
{
    this->player = player;
}

void Player::setOffset(vec2 offset)
{
    this->offset = offset;
}

void Player::removeGameObject()
{
    delete player;
    player = nullptr;
}

//playerPhysicsObject->getRigidBody()->setAngularFactor(btVector3(0, 0, 0));
void Player::update()
{
    lookAction();
    moveAction();

    movePhysics();
}

GameObject* Player::getGameObject() const
{
    return player;
}

vec2 Player::getOffset() const
{
    return offset;
}

Player::~Player()
{
    delete player;
    delete rayTracer;
}
