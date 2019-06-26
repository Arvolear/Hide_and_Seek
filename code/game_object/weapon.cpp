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
    this->twist = quat(0, vec3(0));

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

void Weapon::setTwist(quat twist)
{
    this->twist = twist;
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

    if (getActiveAnimation() || (getActiveAnimation()->getName() != "reload"))
    {
        playAnimation("reload");
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
}

void Weapon::pick()
{
    /* pass soldier deque
     * add to front
     * deactivate physics obj
     * lock rotation
     * play proper reload animation
     * */
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
            Animation* anim = getActiveAnimation();

            if (anim && anim->getName() == "fire")
            {
                if (anim->getCurFrame() + anim->getSpeed() > anim->getEndFrame())
                {
                    /* reset = true */
                    playAnimation("fire");
                    magazineBullets--;

                    return true;
                }
            }
            else
            {
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

/* SETTERS */
/* GETTERS */

vec3 Weapon::getOffset() const
{
    return offset;
}

quat Weapon::getTwist() const
{
    return twist;
}

void Weapon::updateStatus()
{
    Animation* anim = getActiveAnimation();

    if (anim)
    {
        if (anim->getName() == "reload")
        {
            if (anim->getCurFrame() + anim->getSpeed() > anim->getEndFrame())
            {
                int toMove = std::min(storageBullets, magazineSize - magazineBullets);

                storageBullets -= toMove;
                magazineBullets += toMove;
            }
        }
    }
}

void Weapon::updatePosition(vec3 center, vec3 left, vec3 up)
{
    center += normalize(cross(left, up)) * offset.x;
    center += up * offset.y;
    center += left * offset.z;

    setLocalPosition(center, false);
    setLocalRotation(axis(twist), angle(twist), false);
}

Weapon::~Weapon() {}
