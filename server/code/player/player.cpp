#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "player.hpp"

Player::Player(float speed)
{
    this->speed = speed;

    physicsObject = nullptr;
    moveDirection = btVector3(0, 0, 0);
    this->connected = false;
}
        
Player::Player(PhysicsObject* physicsObject, float speed)
{
    this->speed = speed;

    this->physicsObject = physicsObject;
    physicsObject->setUserPointer(this);
    
    setConnected(false);
}

void Player::setConnected(bool connected)
{
    this->connected = connected;

    if (connected)
    {
        if (physicsObject)
        {
            physicsObject->setCollidable(true);
            physicsObject->setStatic(false);
        }
    }
    else
    {
        if (physicsObject)
        {
            physicsObject->setCollidable(false);
            physicsObject->setStatic(true);
        }
    }
}

void Player::setPhysicsObject(PhysicsObject* physicsObject)
{
    this->physicsObject = physicsObject;
    physicsObject->setUserPointer(this);
    setConnected(connected);
}

void Player::removePhysicsObject()
{
    physicsObject->setUserPointer(nullptr);
    physicsObject->setCollidable(true);
    physicsObject->setStatic(false);
    physicsObject = nullptr;
}

void Player::setMoveDirection(btVector3 moveDirection)
{
    this->moveDirection = moveDirection;
}

void Player::update() {}

bool Player::isConnected() const
{
    return connected;
}

PhysicsObject* Player::getPhysicsObject() const
{
    return physicsObject;
}

btVector3 Player::getMoveDirection() const
{
    return moveDirection;
}

float Player::getSpeed() const
{
    return speed;
}

Player::~Player() {}
