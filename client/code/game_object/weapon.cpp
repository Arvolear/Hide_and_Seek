#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../window/glfwevents.hpp"
#include "../window/renderquad.hpp"
#include "../window/window.hpp"

#include "../debug/debugsphere.hpp"
#include "../debug/debugdrawer.hpp"

#include "openglmotionstate.hpp"
#include "animation.hpp"
#include "mesh.hpp"
#include "bone.hpp"
#include "skeleton.hpp"
#include "viewfrustum.hpp"
#include "boundsphere.hpp"
#include "modelloader.hpp"
#include "physicsobject.hpp"
#include "gameobject.hpp"
#include "weapon.hpp"
        
Weapon::Weapon(Window* window, string name) : GameObject(name)
{
    this->window = window;
    
    this->offset = vec3(0);
    this->twist = quat(0, vec3(0, 1, 0));

    this->lastShotTime = 0.0;
    this->storageBullets = 0;
    this->magazineSize = 0;
    this->magazineBullets = 0;

    this->shotSpeed = 0;
    this->shotPower = 0;
}

void Weapon::setOffset(vec3 offset)
{
    this->offset = offset;
}

void Weapon::setTwist(vec3 axis, float angle)
{
    this->twist = quat(angle, axis);
}

void Weapon::setStorageBullets(int storageBullets)
{
    this->storageBullets = storageBullets;
}

void Weapon::setMagazineSize(int magazineSize)
{
    this->magazineSize = magazineSize;

    magazineBullets = std::min(magazineSize, magazineBullets);
}

void Weapon::setMagazineBullets(int magazineBullets)
{
    this->magazineBullets = std::min(magazineBullets, magazineSize);
}

void Weapon::setShotSpeed(float shotSpeed)
{
    this->shotSpeed = shotSpeed;
}

void Weapon::setShotPower(float shotPower)
{
    this->shotPower = shotPower;
}

void Weapon::reload()
{
    if (!storageBullets)
    {
        return;
    }

    if (magazineBullets == magazineSize)
    {
        return;
    }

    playAnimation("reload2");
}

void Weapon::drop(vec3 where)
{
    int power = 3;

    physicsObject->getRigidBody()->applyCentralImpulse(toBtVector3(where) * power); 
    stopAnimation();
    
    setVisible(true);
    physicsObject->setCollidable(true);
    physicsObject->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    setShadow(true);
}

void Weapon::pick(vec3 forward, vec3 up, bool active)
{
    physicsObject->clearTransform();
    
    float angle = 0.0;
    
    vec3 weaponForward = vec3(0, 0, -1);
    vec3 horiForward(forward.x, 0, forward.z);

    if (horiForward != vec3(0, 0, 0))
    {
        horiForward = normalize(horiForward);

        float cosRotAngle = dot(weaponForward, horiForward);

        if (cosRotAngle < -1)
        {
            cosRotAngle = -1;
        }
        else if (cosRotAngle > 1)
        {
            cosRotAngle = 1;
        }

        angle = acos(cosRotAngle); 

        vec3 diff = horiForward - weaponForward;

        if (dot(cross(up, weaponForward), diff) < 0)
        {
            angle *= -1;
        }

        btQuaternion rot(toBtVector3(up), angle);
        physicsObject->setRotation(rot);
    }
    
    weaponForward = vec3(forward.x, 0, forward.z);
    vec3 vertForward = forward;

    if (vertForward != vec3(0, 0, 0))
    {
        weaponForward = normalize(weaponForward);
        vertForward = normalize(vertForward);

        float cosRotAngle = dot(weaponForward, vertForward);

        if (cosRotAngle < -1)
        {
            cosRotAngle = -1;
        }
        else if (cosRotAngle > 1)
        {
            cosRotAngle = 1;
        }

        angle = acos(cosRotAngle); 

        btQuaternion rot(toBtVector3(cross(up, forward)), angle);
        physicsObject->setRotation(rot);
    }

    playAnimation("pick");
    physicsObject->setCollidable(false);
    setVisible(active);
    setShadow(false);
    physicsObject->getRigidBody()->setActivationState(WANTS_DEACTIVATION);

    //physicsObject->setRotation(toBtQuaternion(twist), false);
}

vec3 Weapon::getOffset() const
{
    return offset;
}

quat Weapon::getTwist() const
{
    return twist;
}

int Weapon::getStorageBullets() const
{
    return storageBullets;
}

int Weapon::getMagazineSize() const
{
    return magazineSize;
}

int Weapon::getMagazineBullets() const
{
    return magazineBullets;
}

float Weapon::getShotSpeed() const
{
    return shotSpeed;
}

float Weapon::getShotPower() const
{
    return shotPower;
}

void Weapon::updateStatus()
{
    Animation* anim = getActiveAnimation();

    if (anim)
    {
        if (anim->getName() == "reload1" || anim->getName() == "reload2")
        {
            if (anim->getCurFrame() + anim->getSpeed() > anim->getEndFrame())
            {
                int toMove = std::min(storageBullets, magazineSize - magazineBullets);

                storageBullets -= toMove;
                magazineBullets += toMove;
            }
        }
    }
    else
    {
        playAnimation("idle");
    }

    /*cout << "Storage " << storageBullets << endl;
      cout << "Size " << magazineSize << endl;
      cout << "Magazine " << magazineBullets << endl;
      cout << "shotSpeed " << shotSpeed << endl << endl;*/
}

void Weapon::updateRotation(mat3 rotation)
{
    btQuaternion rot = toBtQuaternion(quat_cast(rotation)); 
    physicsObject->setRotation(rot);
}

void Weapon::updatePosition(vec3 center, vec3 forward, vec3 up)
{
    vec3 left = normalize(cross(up, forward));
    vec3 localUp = normalize(cross(forward, left));

    center += forward * offset.x;
    center += localUp * offset.y;
    center += left * offset.z;

    physicsObject->setPosition(toBtVector3(center), false);
}

Weapon::~Weapon() {}
