#include "../shader/shader.hpp"

#include "../global/globaluse.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "camera.hpp"

#include "../debug/debugdrawer.hpp"

#include "../world/raytracer.hpp"
#include "../world/constrainthandler.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

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
#include "../game_object/weapon.hpp"
#include "../game_object/rifle.hpp"

#include "player.hpp"
#include "soldier.hpp"

Soldier::Soldier(int id, Window* window, vec3 playerPos, vec3 cameraForward, float speed) : Player(id, window, playerPos, cameraForward, speed) 
{
    pickFrom = pickTo = vec3(0);
    dropTo = false;
}
        
Soldier::Soldier(int id, Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed, bool active) : Player(id, window, playerPos, cameraForward, tracer, player, speed, active) 
{
    pickFrom = pickTo = vec3(0);
    dropTo = false;
}

void Soldier::setActive(bool active)
{
    this->active = active;
    setConnected(connected);
}

void Soldier::setConnected(bool connected)
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

        if (!weapons.empty())
        {
            weapons[0]->setVisible(active);
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

        if (!weapons.empty())
        {
            weapons[0]->setVisible(false); 
        }
    }
}

void Soldier::weaponAction()
{
    if (window->isKeyPressedOnce(GLFW_KEY_G))
    {
        drop();
    }

    if (!weapons.empty())
    {
        if (window->isKeyPressedOnce(GLFW_KEY_R))
        {
            weapons[0]->reload(); 
        }

        if (window->isButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            weapons[0]->fire(); 
        }
    }

    if (window->isKeyPressedOnce(GLFW_KEY_F))
    {
        pick();
    }
}

void Soldier::updateWeapon()
{
    if (!weapons.empty())
    {
        weapons[0]->updateStatus();
        weapons[0]->updatePosition(getPosition(), getForward(), getUp());
        weapons[0]->updateRotation(getHorizontalViewRotation());
        weapons[0]->updateRotation(getVerticalViewRotation());
    }
}

void Soldier::drop(Weapon* weapon)
{
    if (weapons.empty() || weapons[0] != weapon)
    {
        return;
    }

    weapons[0]->setUserPointer(nullptr);
    weapons[0]->drop(getForward() + getUp(), active);
    weapons.pop_front();
}

void Soldier::drop()
{
    if (weapons.empty())
    {
        return;
    }

    dropTo = true;
}

void Soldier::pick(Weapon* weapon)
{
    weapon->setUserPointer(this);
    weapon->pick(getForward(), getUp(), active);
    weapons.push_front(weapon); 
}

void Soldier::pick()
{
    if (!(player && player->getPhysicsObject() && rayTracer))
    {
        return;
    }

    pickFrom = getPosition();
    pickTo = getForward();
}

deque < Weapon* > Soldier::getWeapons() const
{
    return weapons;
}

Weapon* Soldier::getWeapon(int index) const
{
    if (index < 0 || index >= (int)weapons.size())
    {
        throw(runtime_error("ERROR::Soldier::getWeapon() index is out of range"));
    }

    return weapons[index];
}

pair < vec3, vec3 > Soldier::getPickRay() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    return {pickFrom, pickTo};
}

bool Soldier::isDrop() const
{   
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    return dropTo;
}

void Soldier::update(bool events)
{
    unique_lock < mutex > lk(mtx);
    ready = false;

    moveDirection = vec3(0, 0, 0);

    if (events && active)
    {
        lookAction();
        moveAction();
        weaponAction();
    }

    if (moveDirection != vec3(0))
    {
        moveDirection = normalize(moveDirection);
    }

    ready = true;
    lk.unlock();
    cv.notify_all();

    movePhysics();

    if (active)
    {
        if (player && player->getPhysicsObject())
        {
            updateCamera();
            updateModel(moveDirection);
            updateAnimation(moveDirection);
        }

        updateWeapon();
    }
}

void Soldier::clearPickData()
{
    pickFrom = pickTo = vec3(0.0);
}

void Soldier::clearDropData()
{   
    dropTo = false;
}

Soldier::~Soldier() {}
