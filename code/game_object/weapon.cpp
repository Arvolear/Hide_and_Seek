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

    /* X move bullets from left to magaz
     * */

    if (!getActiveAnimation() || (getActiveAnimation()->getName() != "reload1"))
    {
        playAnimation("reload1");
    }
}

void Weapon::drop()
{
    /* pass soldier deque
     * remove from front
     * activate physics obj
     * unlock rotation
     * apply impulses (torque + central)
     * */

    stopAnimation();
}

void Weapon::pick()
{
    /* pass soldier deque
     * add to front
     * deactivate physics obj
     * lock rotation
     * play proper reload animation
     * */
    
    physicsObject->setRotation(toBtQuaternion(twist), false);
}

bool Weapon::fire()
{
    /* X check bullets in magaz 
     * check speed or single press 
     * current recoil strick? window?
     * X play proper shot animation 
     * X return true if everything passed ifs (soldier raytracer)
     * */

    /* TO BE CHANGED */
    if (window->isButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        if (magazineBullets)
        {
            if (lastShotTime + shotSpeed <= window->getTime())
            {
                lastShotTime = window->getTime();

                playAnimation("fire");
                magazineBullets--;

                return true; 
            }
        }
        else
        {
            playAnimation("idle");
        }
    }

    return false;
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
