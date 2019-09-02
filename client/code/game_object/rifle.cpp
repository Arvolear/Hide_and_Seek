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
#include "rifle.hpp"

Rifle::Rifle(Window* window, string name) : Weapon(window, name) {}

bool Rifle::fire()
{
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

Rifle::~Rifle() {}
