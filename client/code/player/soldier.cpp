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
    health = 1;
    pickRay = {vec3(0.0), vec3(0.0)};
    dropTo = false;
}
        
Soldier::Soldier(int id, Window* window, vec3 playerPos, vec3 cameraForward, RayTracer* tracer, GameObject* player, float speed, bool active) : Player(id, window, playerPos, cameraForward, tracer, player, speed, active) 
{
    health = 1;
    pickRay = {vec3(0.0), vec3(0.0)};
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

void Soldier::setHealth(int health)
{
    if (!this->health && health)
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
    else if (this->health && !health)
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
    
    this->health = health;
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
            reload();
        }

        if (window->isButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            fire(); 
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

        if (weapons[0]->isReloaded())
        {
            reloadInfo.insert({weapons[0]->getName(), true});
        }
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

    pickRay = {getPosition(), getForward()};
}

void Soldier::fire()
{
    if (dropTo)
    {
        return;
    }
        
    if (weapons[0]->fire())
    { 
        fireInfo[weapons[0]->getName()].push_back({getPosition(), getForward()});
    }
}

void Soldier::reload()
{
    if (dropTo)
    {
        return;
    }
   
    /* check updateWeapon() */
    weapons[0]->reload();
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

pair < vec3, vec3 > Soldier::getPickRay()
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }

    pair < vec3, vec3 > res = pickRay;

    pickRay = {vec3(0.0), vec3(0.0)};

    return res;
}

bool Soldier::isDrop()
{   
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }
    
    bool res = dropTo;
    dropTo = false;

    return res;
}
        
map < string, vector < pair < vec3, vec3 > > > Soldier::getFire()
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }
    
    map < string, vector < pair < vec3, vec3 > > > res = fireInfo;
    fireInfo.clear();

    return res;
}
        
map < string, bool > Soldier::getReload()
{
    unique_lock < mutex > lk(mtx);

    while (!ready)
    {
        cv.wait(lk);
    }
    
    map < string, bool > res = reloadInfo;
    reloadInfo.clear();

    return res; 
}

int Soldier::getHealth() const
{
    return health;
}

void Soldier::update(bool events)
{
    unique_lock < mutex > lk(mtx);
    ready = false;

    moveDirection = vec3(0, 0, 0);

    if (events && active && health)
    {
        lookAction();
        moveAction();
        weaponAction();
    }

    if (moveDirection != vec3(0))
    {
        moveDirection = normalize(moveDirection);
    }

    if (active)
    {
        if (player && player->getPhysicsObject())
        {
            movePhysics();

            updateCamera();
            updateModel(moveDirection);
            updateAnimation(moveDirection);
        }

        updateWeapon();
    }
    
    ready = true;
    cv.notify_all();

   //cout << getPosition().x << ' ' << getPosition().z << endl;
}

Soldier::~Soldier() {}
