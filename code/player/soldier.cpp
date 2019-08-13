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

Soldier::Soldier(Window* window, vec3 playerPos, vec3 cameraForward, float speed) : Player(window, playerPos, cameraForward, speed) {}
        
Soldier::Soldier(Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed, bool active) : Player(window, playerPos, cameraForward, tracer, player, speed, active) {}

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

void Soldier::drop()
{
    if (weapons.empty())
    {
        return;
    }

    weapons[0]->drop(getForward() + getUp());
    weapons.pop_front();
}

void Soldier::pick(Weapon* weapon)
{
    weapon->pick(getForward(), getUp());
    weapons.push_front(weapon); 
}

void Soldier::pick()
{
    if (!(player && player->getPhysicsObject() && rayTracer))
    {
        return;
    }

    btVector3 from = toBtVector3(getPosition());
    btVector3 to = toBtVector3(getForward());

    unique_ptr < RayResult > result(rayTracer->rayCast(from, to, false));

    if (!result.get())
    {
        return;
    }

    float dist = (from - result->hitPoint).length();

    //cout << dist << " " << bottomDist << endl; 

    int optimalDistance = 20;

    if (dist > optimalDistance)
    {
        return;
    }

    Weapon* hitOne = dynamic_cast < Weapon* >(static_cast < GameObject* >(static_cast < PhysicsObject* >(result->body->getUserPointer())->getUserPointer()));

    if (!hitOne)
    {
        return;
    }

    pick(hitOne);
}

void Soldier::update(bool events)
{
    if (events && active)
    {
        lookAction();
        moveAction();
        weaponAction();
    }

    movePhysics();

    updateWeapon();
}

Soldier::~Soldier() {}
