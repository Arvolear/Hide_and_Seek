#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../world/raytracer.hpp"

#include "player.hpp"

Player::Player(float speed)
{
    this->speed = speed;

    rayTracer = nullptr;
    physicsObject = nullptr;

    jumpAllowed = true;
    speedLock = false;

    Forward = Left = Up = headCenter = btVector3(0, 0, 0);
}
        
Player::Player(RayTracer* tracer, PhysicsObject* physicsObject, float speed)
{
    this->speed = speed;

    rayTracer = tracer;
    this->physicsObject = physicsObject;
    
    jumpAllowed = true;
    speedLock = false;
    
    Forward = Left = Up = headCenter= btVector3(0, 0, 0);
}

/*
dist > 2.2
*/

bool Player::isGroundStanding()
{
    if (!(physicsObject && rayTracer))
    {
        return false;
    }

    btVector3 from = physicsObject->getRigidBody()->getCenterOfMassPosition();
    btVector3 to = btVector3(0, -1, 0);

    unique_ptr < RayResult > result(rayTracer->rayCast(from, to, false));

    if (!result)
    {
        return false;
    }

    float dist = (from - result->hitPoint).length();

    btVector3 Min;
    btVector3 Max;

    physicsObject->getRigidBody()->getAabb(Min, Max);

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
    if (!(physicsObject && rayTracer && jumpAllowed))
    {
        return;
    }

    float power = 70;
    float loss = 0.5;

    //cout << "jump" << endl;

    physicsObject->getRigidBody()->setActivationState(ACTIVE_TAG);

    /* redusing the speed */
    btVector3 velocity = physicsObject->getRigidBody()->getLinearVelocity();
    velocity = btVector3(velocity.x() * loss, velocity.y(), velocity.z() * loss);
    physicsObject->getRigidBody()->setLinearVelocity(velocity);

    physicsObject->getRigidBody()->applyCentralImpulse(btVector3(0, 1, 0) * power);

    jumpAllowed = false;
}

void Player::moveAction()
{
    btVector3 playerForward = Left.cross(Up).normalize();

    if (move.find("W") != string::npos)
    {
        if (physicsObject && rayTracer)
        {
            moveDirection += playerForward;
        }
        else
        {
            moveDirection += Forward;
        }
    }

    if (move.find("S") != string::npos)
    {
        if (physicsObject && rayTracer)
        {
            moveDirection -= playerForward;
        }
        else
        {
            moveDirection -= Forward;
        }
    }

    if (move.find("D") != string::npos)
    {
        moveDirection -= Left;
    }

    if (move.find("A") != string::npos)
    {
        moveDirection += Left;
    }

    if (physicsObject && rayTracer)
    {
        if (move.find("J") != string::npos)
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
    physicsObject->getRigidBody()->applyCentralImpulse(btVector3(moveDirection.x(), 0, moveDirection.z()) * speed * speedFactor);

    btVector3 velocity = physicsObject->getRigidBody()->getLinearVelocity();

    /* disable sliding effect */
    if (velocity.length() < 0.1)
    {
        physicsObject->getRigidBody()->setActivationState(WANTS_DEACTIVATION);
    }

    /* friction */
    velocity = btVector3(velocity.x() * friction, velocity.y(), velocity.z() * friction);

    physicsObject->getRigidBody()->setLinearVelocity(velocity);

    jumpAllowed = true;
}

void Player::moveAir()
{
    float speedFactor = 2.2;

    jumpAllowed = false;

    /* push the body */
    physicsObject->getRigidBody()->applyCentralImpulse(btVector3(moveDirection.x(), 0, moveDirection.z()) * speed * speedFactor);

    btVector3 velocity = physicsObject->getRigidBody()->getLinearVelocity();

    /* enable sliding effect */
    if (abs(velocity.y()) < 0.1)
    {
        physicsObject->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    }
}

void Player::speedHackControl()
{
    float maxSpeed = 6;

    /* speed hack control */
    btVector3 velocity = physicsObject->getRigidBody()->getLinearVelocity();
    btVector3 XZVelocity = velocity;
    XZVelocity.setY(0);

    if (XZVelocity.length() > maxSpeed)
    {
        btVector3 XZNormVel = XZVelocity;
        XZNormVel.normalize();

        velocity -= (XZVelocity.length() - maxSpeed) * XZNormVel; 

        physicsObject->getRigidBody()->setLinearVelocity(velocity);
    }
}

void Player::calcCameraPosition()
{
    btTransform globalTransform;
    btTransform headTransform;
    btVector3 globalCenter;
    btScalar headRadius;

    globalTransform = physicsObject->getRigidBody()->getCenterOfMassTransform();
    headTransform = physicsObject->getCompoundShape()->getChildTransform(0);
    physicsObject->getCompoundShape()->getChildShape(0)->getBoundingSphere(headCenter, headRadius);

    globalCenter = physicsObject->getRigidBody()->getCenterOfMassPosition();
    
    /* head center (camera) */
    headCenter = globalTransform * headTransform * headCenter;

    //cout << headCenter.x() << ' ' << headCenter.y() << ' ' << headCenter.z() << endl;
    
    /* new Up */
    //Up = (headCenter - globalCenter).normalize();

    physicsObject->getRigidBody()->forceActivationState(ACTIVE_TAG); 
}

void Player::movePhysics()
{
    moveAction();

    if (moveDirection != btVector3(0, 0, 0))
    {
        moveDirection = moveDirection.normalize(); // normalizing move direction for equal speeds
    }

    /* if physics object is applied */
    if (physicsObject && rayTracer)
    {
        if (!physicsObject->getCompoundShape())
        {
            throw runtime_error("ERROR::player no compound shape"); 
        }

        if (physicsObject->getCompoundShape()->getShape()->getNumChildShapes() < 1)
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
        
        /* client camera position */
        calcCameraPosition();
    }

    moveDirection = btVector3(0, 0, 0);
    move = "";
}

void Player::setRayTracer(RayTracer* tracer)
{
    this->rayTracer = tracer;
}

void Player::setPhysicsObject(PhysicsObject* physicsObject)
{
    this->physicsObject = physicsObject;
}

void Player::setForward(btVector3 Forward)
{
    this->Forward = Forward.normalize();
}

void Player::setLeft(btVector3 Left)
{
    this->Left = Left.normalize();
}

void Player::setUp(btVector3 Up)
{
    this->Up = Up.normalize();
}
        
void Player::setMovement(string move)
{
    this->move = move;
}

void Player::removePhysicsObject()
{
    physicsObject = nullptr;
}

void Player::update()
{
    movePhysics();
}

PhysicsObject* Player::getPhysicsObject() const
{
    return physicsObject;
}

btVector3 Player::getHeadCenter() const
{
    return headCenter;
}

Player::~Player()
{
    delete rayTracer;
}
