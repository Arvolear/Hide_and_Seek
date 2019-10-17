#include "global/convert.hpp"

#include "world/raytracer.hpp"
#include "world/bulletevents.hpp"
#include "world/world.hpp"

#include "physics_object/openglmotionstate.hpp"
#include "physics_object/physicsobject.hpp"
#include "physics_object/weapon.hpp"

#include "player/player.hpp"
#include "player/soldier.hpp"

#include "level/levelloader.hpp"
#include "level/level.hpp"

#include "multiplayer/node.hpp"
#include "multiplayer/playerdatacollector.hpp"
#include "multiplayer/playerdataupdater.hpp"
#include "multiplayer/physicsobjectdatacollector.hpp"
#include "multiplayer/physicsobjectdataupdater.hpp"
#include "multiplayer/weapondatacollector.hpp"
#include "multiplayer/weaponpickercollector.hpp"
#include "multiplayer/weaponpickerupdater.hpp"
#include "multiplayer/weapondroppercollector.hpp"
#include "multiplayer/weapondropperupdater.hpp"
#include "multiplayer/playerconnectioncollector.hpp"
#include "multiplayer/playerdisconnectioncollector.hpp"
#include "multiplayer/multiplayer.hpp"

#include "game/game.hpp"

int main()
{
    Game* G = new Game("urban");

    G->gameLoop();

    delete G;

    return 0;
}
