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
}

Weapon::Weapon(Window* window, vec3 offset, quat twist, string name, bool visible, string path, PhysicsObject* physicsObject, ViewFrustum* viewFrustum) : GameObject(name, visible, path, physicsObject, viewFrustum)
{
    this->window = window;
    this->offset = offset;
    this->twist = twist;
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
    if (!bulletsLeft)
    {
        return;
    }

    /* move bullets from left to magaz */

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
     * apply impulses (torque + central)
     * */
}

void Weapon::pick()
{
    /* pass soldier deque
     * add to front
     * deactivate physics obj
     * play proper reload animation
     * */
}

void Weapon::fire()
{
    /* check bullets in magaz
     * check speed
     * current recoil strick? window?
     * play proper shot animation 
     * return true if everything passed ifs (soldier raytracer)
     * */
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

void Weapon::updatePosition(vec3 center, vec3 left, vec3 up)
{
    center += normalize(cross(left, up)) * offset.x;
    center += up * offset.y;
    center += left * offset.z;

    setLocalPosition(center, false);
    setLocalRotation(axis(twist), angle(twist), false);
}

Weapon::~Weapon() {}
