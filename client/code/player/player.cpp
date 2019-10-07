#include "../shader/shader.hpp"

#include "../global/globaluse.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "camera.hpp"

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

#include "../world/raytracer.hpp"

#include "player.hpp"

Player::Player(Window* window, vec3 playerPos, vec3 cameraForward, float speed) : Camera(window, playerPos, cameraForward, speed)
{
    rayTracer = nullptr;
    player = nullptr;

    jumpAllowed = true;
    speedLock = false;

    cameraOffset = modelOffset = vec3(0);
    modelForward = normalize(cross(Left, Up));

    this->connected = false;
    this->active = false;
}
        
Player::Player(Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed, bool active) : Camera(window, playerPos, cameraForward, speed)
{
    rayTracer = tracer;
    this->player = player;
    
    jumpAllowed = true;
    speedLock = false;
    
    cameraOffset = modelOffset = vec3(0);
    modelForward = normalize(cross(Left, Up));
    
    setConnected(false);
    this->active = active;
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

    unique_ptr < RayResult > result(rayTracer->rayCast(from, to, false));

    if (!result)
    {
        return false;
    }

    float dist = (from - result->hitPoint).length();

    btVector3 Min;
    btVector3 Max;

    player->getPhysicsObject()->getRigidBody()->getAabb(Min, Max);

    float bottomDist = (from - btVector3(from.x(), Min.y(), from.z())).length();

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
    if (abs(velocity.y()) < 0.05)
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
    }
    else
    {
        Pos += moveDirection * vec3(speed);
    }
}

void Player::setConnected(bool connected)
{
    this->connected = connected;

    if (connected)
    {
        if (player)
        {
            player->setVisible(!active);
            player->setCollidable(true);
            player->setStatic(false);
            updateModel(vec3(0));
        }
    }
    else
    {
        if (player)
        {
            player->setVisible(false);
            player->setCollidable(false);
            player->setStatic(true);
        }
    }
}

void Player::setActive(bool active)
{
    this->active = active;
    setConnected(connected);
}

void Player::setRayTracer(RayTracer* tracer)
{
    this->rayTracer = tracer;
}

void Player::setGameObject(GameObject* player)
{
    player->setUserPointer(this);
    this->player = player;
    setConnected(connected);
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
    player->setVisible(!active);
    player->setCollidable(true);
    player->setStatic(false);
    player = nullptr;
}

bool Player::isActive() const
{
    return active;
}

bool Player::isConnected() const
{
    return connected;
}

void Player::updateCamera()
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

    /* camera Position */
    setPosition(headCenter.x(), headCenter.y(), headCenter.z());

    /* new Up */
    Up = normalize(global.toVec3(headCenter) - global.toVec3(globalCenter));

    Pos += normalize(cross(Left, Up)) * cameraOffset.x;
    Pos += Up * cameraOffset.y;
    Pos += Left * cameraOffset.z;

    player->getPhysicsObject()->getRigidBody()->forceActivationState(ACTIVE_TAG);
}

void Player::updateModel(vec3 newForward)
{
    float angle = 0.0;

    if (newForward != vec3(0))
    {
        modelForward = normalize(modelForward);
        newForward = normalize(newForward);

        float cosRotAngle = dot(modelForward, newForward);

        if (cosRotAngle < -1)
        {
            cosRotAngle = -1;
        }
        else if (cosRotAngle > 1)
        {
            cosRotAngle = 1;
        }

        angle = global.toDegs(acos(cosRotAngle)); 

        vec3 diff = newForward - modelForward;

        if (dot(cross(Up, modelForward), diff) < 0)
        {
            angle *= -1;
        }

        modelForward = newForward;
    }

    if (!active)
    {
        Forward = modelForward;
        Left = normalize(cross(Up, Forward));
    }

    player->setLocalRotation(Up, angle);

    vec3 localPos = vec3(0);

    localPos += modelForward * modelOffset.x;
    localPos += Up * modelOffset.y;
    localPos += Left * modelOffset.z;

    player->setLocalPosition(localPos, false);
}

void Player::updateAnimation(vec3 moveDirection)
{
    if (!player->getActiveAnimation() || (player->getActiveAnimation()->getName() != "idle" && (moveDirection == vec3(0) || !isGroundStanding())))
    {
        player->playAnimation("idle");
    }
    else if (player->getActiveAnimation() && player->getActiveAnimation()->getName() != "run" && moveDirection != vec3(0) && isGroundStanding())
    {
        player->playAnimation("run");
    }
}

void Player::update(bool events)
{
    unique_lock < mutex > lk(mtx);
    ready = false;

    moveDirection = vec3(0, 0, 0);

    if (events && active)
    {
        lookAction();
        moveAction();
    }

    if (moveDirection != vec3(0))
    {
        moveDirection = normalize(moveDirection);
    }

    ready = true;
    lk.unlock();
    cv.notify_all();

    if (active && player && player->getPhysicsObject())
    {
        movePhysics();

        updateCamera();
        updateModel(moveDirection);
        updateAnimation(moveDirection);
    }
}

GameObject* Player::getGameObject() const
{
    return player;
}

vec3 Player::getCameraOffset() const
{
    return cameraOffset;
}

vec3 Player::getModelOffset() const
{
    return modelOffset;
}

vec3 Player::getModelForward() const
{
    return modelForward;
}

Player::~Player()
{
    delete rayTracer;
}
