#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "player.hpp"

Player::Player(float speed)
{
    this->speed = speed;

    physicsObject = nullptr;
    moveDirection = btVector3(0, 0, 0);
}
        
Player::Player(PhysicsObject* physicsObject, float speed)
{
    this->speed = speed;

    this->physicsObject = physicsObject;
}

void Player::setPhysicsObject(PhysicsObject* physicsObject)
{
    this->physicsObject = physicsObject;
}

void Player::removePhysicsObject()
{
    physicsObject = nullptr;
}
        
void Player::setMoveDirection(btVector3 moveDirection)
{
    this->moveDirection = moveDirection;
}

void Player::update() {}

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
