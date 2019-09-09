#include "../shader/shader.hpp"

#include "../global/convert.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "camera.hpp"

#include "../debug/debugsphere.hpp"
#include "../debug/debugdrawer.hpp"

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

#include "../world/raytracer.hpp"

#include "player.hpp"
#include "soldier.hpp"

Soldier::Soldier(Window* window, vec3 playerPos, vec3 cameraForward, float speed) : Player(window, playerPos, cameraForward, speed) 
{
    pickFrom = pickTo = vec3(0);
}
        
Soldier::Soldier(Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed, bool active) : Player(window, playerPos, cameraForward, tracer, player, speed, active) 
{
    pickFrom = pickTo = vec3(0);
}

void Soldier::setActive(bool active)
{
    this->active = active;

    if (player)
    {
        player->setVisible(!active);
    }

    if (!weapons.empty())
    {
        weapons[0]->setVisible(active);
    }
}

void Soldier::weaponAction()
{
    pickFrom = pickTo = vec3(0);

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

void Soldier::drop()
{
    if (weapons.empty())
    {
        return;
    }
   
    dropTo = true;

    weapons[0]->setUserPointer(nullptr);
    weapons[0]->drop(getForward() + getUp());
    weapons.pop_front();
}

void Soldier::pick(Weapon* weapon)
{
    weapon->pick(getForward(), getUp(), active);
    weapons.push_front(weapon); 
    weapon->setUserPointer(this);
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

pair < vec3, vec3 > Soldier::getPickRay() const
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    return {pickFrom, pickTo};
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
