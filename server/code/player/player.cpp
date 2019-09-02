#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "player.hpp"

Player::Player(float speed)
{
    this->speed = speed;

    physicsObject = nullptr;
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

void Player::update() {}

PhysicsObject* Player::getPhysicsObject() const
{
    return physicsObject;
}

float Player::getSpeed() const
{
    return speed;
}

Player::~Player() {}
