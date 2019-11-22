#include "global/globaluse.hpp"

#include "shader/shader.hpp"

#include "framebuffer/framebuffer.hpp"
#include "framebuffer/colorbuffer.hpp"
#include "framebuffer/depthbuffer.hpp"
#include "framebuffer/shadowbuffer.hpp"
#include "framebuffer/gbuffer.hpp"

#include "window/glfwevents.hpp"
#include "window/renderquad.hpp"
#include "window/window.hpp"

#include "global/radialblur.hpp"
#include "global/gaussianblur.hpp"
#include "global/poissondisk.hpp"

#include "player/camera.hpp"

#include "debug/debugdrawer.hpp"

#include "world/raytracer.hpp"
#include "world/constrainthandler.hpp"
#include "world/bulletevents.hpp"
#include "world/world.hpp"

#include "game_object/sphere.hpp"
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
#include "game_object/instancedgameobject.hpp"
#include "game_object/weapon.hpp"
#include "game_object/rifle.hpp"

#include "player/player.hpp"

#include "multiplayer/client.hpp"
#include "multiplayer/playerdatacollector.hpp"

#include "level/bloom.hpp"
#include "level/lensflare.hpp"
#include "level/dirlightsoftshadow.hpp"
#include "level/dirlight.hpp"
#include "level/ssao.hpp"
#include "level/atmosphere.hpp"
#include "level/skybox.hpp"
#include "level/levelloader.hpp"
#include "level/level.hpp"

#include "multiplayer/client.hpp"
#include "multiplayer/physicsobjectdataparser.hpp"
#include "multiplayer/playerdatacollector.hpp"
#include "multiplayer/playerdataupdater.hpp"
#include "multiplayer/gameobjectdatacollector.hpp"
#include "multiplayer/gameobjectdataupdater.hpp"
#include "multiplayer/weapondataupdater.hpp"
#include "multiplayer/weaponpickercollector.hpp"
#include "multiplayer/weaponpickerupdater.hpp"
#include "multiplayer/weapondroppercollector.hpp"
#include "multiplayer/weapondropperupdater.hpp"
#include "multiplayer/playerconnectionupdater.hpp"
#include "multiplayer/playerdisconnectionupdater.hpp"
#include "multiplayer/multiplayer.hpp"

#include "game/game.hpp"

#include "menu/menu.hpp"

int main()
{
    Menu* M = new Menu();

    M->menuLoop();

    delete M;

    return 0;
}
