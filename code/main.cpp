#include "global/convert.hpp"

#include "shader/shader.hpp"

#include "framebuffer/framebuffer.hpp"
#include "framebuffer/colorbuffer.hpp"
#include "framebuffer/depthbuffer.hpp"

#include "window/glfwevents.hpp"
#include "window/renderquad.hpp"
#include "window/window.hpp"

#include "player/camera.hpp"

#include "debug/debugsphere.hpp"
#include "debug/debugdrawer.hpp"

#include "world/raytracer.hpp"
#include "world/constrainthandler.hpp"
#include "world/bulletevents.hpp"
#include "world/world.hpp"

#include "game_object/openglmotionstate.hpp"
#include "game_object/animation.hpp"
#include "game_object/mesh.hpp"
#include "game_object/bone.hpp"
#include "game_object/skeleton.hpp"
#include "game_object/viewfrustum.hpp"
#include "game_object/boundsphere.hpp"
#include "game_object/modelloader.hpp"
#include "game_object/physicsobject.hpp"
#include "game_object/gameobject.hpp"
#include "game_object/weapon.hpp"

#include "player/player.hpp"

#include "level/dirlight.hpp"
#include "level/skybox.hpp"
#include "level/levelloader.hpp"
#include "level/level.hpp"

#include "game/game.hpp"

#include "menu/menu.hpp"

int main()
{
    Menu* M = new Menu();

    M->menuLoop();

    delete M;

    return 0;
}
