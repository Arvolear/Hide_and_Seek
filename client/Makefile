.PHONY: release all clean cleanall

FLAGS = -std=c++11 -Wall -O3

LIBS = -lGLEW -lGL -lSOIL -lassimp -lglfw3 -lX11 -lpthread -ldl -lBulletCollision -lBulletDynamics -lLinearMath -ltinyxml2

RELEASE = -Wl,-rpath,./lib

INPUTDIR = ./code
OUTPUTDIR = ./build

MAIN = main.o 
GLOBAL = global.o fpscounter.o gaussianblur.o radialblur.o
DEBUG = debugdrawer.o
SHADER = shader.o 
FRAMEBUFFER = framebuffer.o colorbuffer.o depthbuffer.o shadowbuffer.o gbuffer.o
WINDOW = window.o glfwevents.o renderquad.o 
MENU = menu.o 
GAME = game.o
MULTIPLAYER = multiplayer.o client.o playerdatacollector.o playerdataupdater.o gameobjectdatacollector.o gameobjectdataupdater.o weaponpickercollector.o weaponpickerupdater.o weapondroppercollector.o weapondropperupdater.o playerconnectionupdater.o playerdisconnectionupdater.o
LEVEL = level.o dirlight.o dirlightsoftshadow.o skybox.o atmosphere.o levelloader.o
WORLD = world.o bulletevents.o constrainthandler.o raytracer.o
PLAYER = camera.o player.o soldier.o
GAME_OBJECT = rifle.o weapon.o gameobject.o physicsobject.o openglmotionstate.o modelloader.o viewfrustum.o boundsphere.o skeleton.o bone.o mesh.o animation.o sphere.o

OBJECTFILES = $(addprefix $(OUTPUTDIR)/, $(MAIN) $(GLOBAL) $(DEBUG) $(SHADER) $(FRAMEBUFFER) $(WINDOW) $(MENU) $(GAME) $(MULTIPLAYER) $(LEVEL) $(WORLD) $(PLAYER) $(GAME_OBJECT)) 

### ALL ###

all: Hide_and_Seek

Hide_and_Seek: $(OBJECTFILES)
	g++ -o Hide_and_Seek $(OBJECTFILES) $(LIBS) $(FLAGS) 

### RELEASE ###

release: $(OBJECTFILES)
	g++ -o Hide_and_Seek $(OBJECTFILES) $(RELEASE) $(LIBS) $(FLAGS)

### MAIN ###

$(OUTPUTDIR)/main.o: $(INPUTDIR)/main.cpp
	g++ -c $(INPUTDIR)/main.cpp -o $@ $(FLAGS)

### GLOBAL ###

$(OUTPUTDIR)/global.o: $(INPUTDIR)/global/global.cpp $(INPUTDIR)/global/global.hpp
	g++ -c $(INPUTDIR)/global/global.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/fpscounter.o: $(INPUTDIR)/global/fpscounter.cpp $(INPUTDIR)/global/fpscounter.hpp
	g++ -c $(INPUTDIR)/global/fpscounter.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/gaussianblur.o: $(INPUTDIR)/global/gaussianblur.cpp $(INPUTDIR)/global/gaussianblur.hpp
	g++ -c $(INPUTDIR)/global/gaussianblur.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/radialblur.o: $(INPUTDIR)/global/radialblur.cpp $(INPUTDIR)/global/radialblur.hpp
	g++ -c $(INPUTDIR)/global/radialblur.cpp -o $@ $(FLAGS)

### DEBUG ###

$(OUTPUTDIR)/debugdrawer.o: $(INPUTDIR)/debug/debugdrawer.cpp $(INPUTDIR)/debug/debugdrawer.hpp
	g++ -c $(INPUTDIR)/debug/debugdrawer.cpp -o $@ $(FLAGS)

### SHADER ###

$(OUTPUTDIR)/shader.o: $(INPUTDIR)/shader/shader.cpp $(INPUTDIR)/shader/shader.hpp
	g++ -c $(INPUTDIR)/shader/shader.cpp -o $@ $(FLAGS)

### FRAMEBUFFER ###

$(OUTPUTDIR)/framebuffer.o: $(INPUTDIR)/framebuffer/framebuffer.cpp $(INPUTDIR)/framebuffer/framebuffer.hpp
	g++ -c $(INPUTDIR)/framebuffer/framebuffer.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/colorbuffer.o: $(INPUTDIR)/framebuffer/colorbuffer.cpp $(INPUTDIR)/framebuffer/colorbuffer.hpp
	g++ -c $(INPUTDIR)/framebuffer/colorbuffer.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/depthbuffer.o: $(INPUTDIR)/framebuffer/depthbuffer.cpp $(INPUTDIR)/framebuffer/depthbuffer.hpp
	g++ -c $(INPUTDIR)/framebuffer/depthbuffer.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/shadowbuffer.o: $(INPUTDIR)/framebuffer/shadowbuffer.cpp $(INPUTDIR)/framebuffer/shadowbuffer.hpp
	g++ -c $(INPUTDIR)/framebuffer/shadowbuffer.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/gbuffer.o: $(INPUTDIR)/framebuffer/gbuffer.cpp $(INPUTDIR)/framebuffer/gbuffer.hpp
	g++ -c $(INPUTDIR)/framebuffer/gbuffer.cpp -o $@ $(FLAGS)

### WINDOW ###

$(OUTPUTDIR)/window.o: $(INPUTDIR)/window/window.cpp $(INPUTDIR)/window/window.hpp
	g++ -c $(INPUTDIR)/window/window.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/glfwevents.o: $(INPUTDIR)/window/glfwevents.cpp $(INPUTDIR)/window/glfwevents.hpp
	g++ -c $(INPUTDIR)/window/glfwevents.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/renderquad.o: $(INPUTDIR)/window/renderquad.cpp $(INPUTDIR)/window/renderquad.hpp
	g++ -c $(INPUTDIR)/window/renderquad.cpp -o $@ $(FLAGS)

### MENU ###

$(OUTPUTDIR)/menu.o: $(INPUTDIR)/menu/menu.cpp $(INPUTDIR)/menu/menu.hpp
	g++ -c $(INPUTDIR)/menu/menu.cpp -o $@ $(FLAGS)

### GAME ###

$(OUTPUTDIR)/game.o: $(INPUTDIR)/game/game.cpp $(INPUTDIR)/game/game.hpp
	g++ -c $(INPUTDIR)/game/game.cpp -o $@ $(FLAGS)

### MULTIPLAYER ###

$(OUTPUTDIR)/multiplayer.o: $(INPUTDIR)/multiplayer/multiplayer.cpp $(INPUTDIR)/multiplayer/multiplayer.hpp
	g++ -c $(INPUTDIR)/multiplayer/multiplayer.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/client.o: $(INPUTDIR)/multiplayer/client.cpp $(INPUTDIR)/multiplayer/client.hpp
	g++ -c $(INPUTDIR)/multiplayer/client.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/playerdatacollector.o: $(INPUTDIR)/multiplayer/playerdatacollector.cpp $(INPUTDIR)/multiplayer/playerdatacollector.hpp
	g++ -c $(INPUTDIR)/multiplayer/playerdatacollector.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/playerdataupdater.o: $(INPUTDIR)/multiplayer/playerdataupdater.cpp $(INPUTDIR)/multiplayer/playerdataupdater.hpp
	g++ -c $(INPUTDIR)/multiplayer/playerdataupdater.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/gameobjectdatacollector.o: $(INPUTDIR)/multiplayer/gameobjectdatacollector.cpp $(INPUTDIR)/multiplayer/gameobjectdatacollector.hpp
	g++ -c $(INPUTDIR)/multiplayer/gameobjectdatacollector.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/gameobjectdataupdater.o: $(INPUTDIR)/multiplayer/gameobjectdataupdater.cpp $(INPUTDIR)/multiplayer/gameobjectdataupdater.hpp
	g++ -c $(INPUTDIR)/multiplayer/gameobjectdataupdater.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/weaponpickercollector.o: $(INPUTDIR)/multiplayer/weaponpickercollector.cpp $(INPUTDIR)/multiplayer/weaponpickercollector.hpp
	g++ -c $(INPUTDIR)/multiplayer/weaponpickercollector.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/weaponpickerupdater.o: $(INPUTDIR)/multiplayer/weaponpickerupdater.cpp $(INPUTDIR)/multiplayer/weaponpickerupdater.hpp
	g++ -c $(INPUTDIR)/multiplayer/weaponpickerupdater.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/weapondroppercollector.o: $(INPUTDIR)/multiplayer/weapondroppercollector.cpp $(INPUTDIR)/multiplayer/weapondroppercollector.hpp
	g++ -c $(INPUTDIR)/multiplayer/weapondroppercollector.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/weapondropperupdater.o: $(INPUTDIR)/multiplayer/weapondropperupdater.cpp $(INPUTDIR)/multiplayer/weapondropperupdater.hpp
	g++ -c $(INPUTDIR)/multiplayer/weapondropperupdater.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/playerconnectionupdater.o: $(INPUTDIR)/multiplayer/playerconnectionupdater.cpp $(INPUTDIR)/multiplayer/playerconnectionupdater.hpp
	g++ -c $(INPUTDIR)/multiplayer/playerconnectionupdater.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/playerdisconnectionupdater.o: $(INPUTDIR)/multiplayer/playerdisconnectionupdater.cpp $(INPUTDIR)/multiplayer/playerdisconnectionupdater.hpp
	g++ -c $(INPUTDIR)/multiplayer/playerdisconnectionupdater.cpp -o $@ $(FLAGS)

### LEVEL ###

$(OUTPUTDIR)/level.o: $(INPUTDIR)/level/level.cpp $(INPUTDIR)/level/level.hpp
	g++ -c $(INPUTDIR)/level/level.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/dirlight.o: $(INPUTDIR)/level/dirlight.cpp $(INPUTDIR)/level/dirlight.hpp
	g++ -c $(INPUTDIR)/level/dirlight.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/dirlightsoftshadow.o: $(INPUTDIR)/level/dirlightsoftshadow.cpp $(INPUTDIR)/level/dirlightsoftshadow.hpp
	g++ -c $(INPUTDIR)/level/dirlightsoftshadow.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/skybox.o: $(INPUTDIR)/level/skybox.cpp $(INPUTDIR)/level/skybox.hpp
	g++ -c $(INPUTDIR)/level/skybox.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/atmosphere.o: $(INPUTDIR)/level/atmosphere.cpp $(INPUTDIR)/level/atmosphere.hpp
	g++ -c $(INPUTDIR)/level/atmosphere.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/levelloader.o: $(INPUTDIR)/level/levelloader.cpp $(INPUTDIR)/level/levelloader.hpp
	g++ -c $(INPUTDIR)/level/levelloader.cpp -o $@ $(FLAGS)

### WORLD ###

$(OUTPUTDIR)/world.o: $(INPUTDIR)/world/world.cpp $(INPUTDIR)/world/world.hpp
	g++ -c $(INPUTDIR)/world/world.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/bulletevents.o: $(INPUTDIR)/world/bulletevents.cpp $(INPUTDIR)/world/bulletevents.hpp
	g++ -c $(INPUTDIR)/world/bulletevents.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/constrainthandler.o: $(INPUTDIR)/world/constrainthandler.cpp $(INPUTDIR)/world/constrainthandler.hpp
	g++ -c $(INPUTDIR)/world/constrainthandler.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/raytracer.o: $(INPUTDIR)/world/raytracer.cpp $(INPUTDIR)/world/raytracer.hpp
	g++ -c $(INPUTDIR)/world/raytracer.cpp -o $@ $(FLAGS)

### PLAYER ###

$(OUTPUTDIR)/camera.o: $(INPUTDIR)/player/camera.cpp $(INPUTDIR)/player/camera.hpp
	g++ -c $(INPUTDIR)/player/camera.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/player.o: $(INPUTDIR)/player/player.cpp $(INPUTDIR)/player/player.hpp
	g++ -c $(INPUTDIR)/player/player.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/soldier.o: $(INPUTDIR)/player/soldier.cpp $(INPUTDIR)/player/soldier.hpp
	g++ -c $(INPUTDIR)/player/soldier.cpp -o $@ $(FLAGS)

### GAME_OBJECT ###

$(OUTPUTDIR)/rifle.o: $(INPUTDIR)/game_object/rifle.cpp $(INPUTDIR)/game_object/rifle.hpp
	g++ -c $(INPUTDIR)/game_object/rifle.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/weapon.o: $(INPUTDIR)/game_object/weapon.cpp $(INPUTDIR)/game_object/weapon.hpp
	g++ -c $(INPUTDIR)/game_object/weapon.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/gameobject.o: $(INPUTDIR)/game_object/gameobject.cpp $(INPUTDIR)/game_object/gameobject.hpp
	g++ -c $(INPUTDIR)/game_object/gameobject.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/physicsobject.o: $(INPUTDIR)/game_object/physicsobject.cpp $(INPUTDIR)/game_object/physicsobject.hpp
	g++ -c $(INPUTDIR)/game_object/physicsobject.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/openglmotionstate.o: $(INPUTDIR)/game_object/openglmotionstate.cpp $(INPUTDIR)/game_object/openglmotionstate.hpp
	g++ -c $(INPUTDIR)/game_object/openglmotionstate.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/modelloader.o: $(INPUTDIR)/game_object/modelloader.cpp $(INPUTDIR)/game_object/modelloader.hpp
	g++ -c $(INPUTDIR)/game_object/modelloader.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/viewfrustum.o: $(INPUTDIR)/game_object/viewfrustum.cpp $(INPUTDIR)/game_object/viewfrustum.hpp
	g++ -c $(INPUTDIR)/game_object/viewfrustum.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/boundsphere.o: $(INPUTDIR)/game_object/boundsphere.cpp $(INPUTDIR)/game_object/boundsphere.hpp
	g++ -c $(INPUTDIR)/game_object/boundsphere.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/skeleton.o: $(INPUTDIR)/game_object/skeleton.cpp $(INPUTDIR)/game_object/skeleton.hpp
	g++ -c $(INPUTDIR)/game_object/skeleton.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/bone.o: $(INPUTDIR)/game_object/bone.cpp $(INPUTDIR)/game_object/bone.hpp
	g++ -c $(INPUTDIR)/game_object/bone.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/mesh.o: $(INPUTDIR)/game_object/mesh.cpp $(INPUTDIR)/game_object/mesh.hpp
	g++ -c $(INPUTDIR)/game_object/mesh.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/animation.o: $(INPUTDIR)/game_object/animation.cpp $(INPUTDIR)/game_object/animation.hpp
	g++ -c $(INPUTDIR)/game_object/animation.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/sphere.o: $(INPUTDIR)/game_object/sphere.cpp $(INPUTDIR)/game_object/sphere.hpp
	g++ -c $(INPUTDIR)/game_object/sphere.cpp -o $@ $(FLAGS)

### CLEAN ###

clean:
	rm -rf $(OUTPUTDIR)/*.o

### CLEANALL ###

cleanall:
	rm -rf Hide_and_Seek
	rm -rf $(OUTPUTDIR)/*.o
