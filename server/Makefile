.PHONY: release all clean cleanall

FLAGS = -std=c++11 -Wall -O3

LIBS = -lpthread -lBulletCollision -lBulletDynamics -lLinearMath -ltinyxml2

RELEASE = -Wl,-rpath,./lib

INPUTDIR = ./code
OUTPUTDIR = ./build

MAIN = main.o 
GAME = game.o
MULTIPLAYER = multiplayer.o node.o playerdatacollector.o playerdataupdater.o physicsobjectdatacollector.o physicsobjectdataupdater.o weaponpickercollector.o weaponpickerupdater.o
LEVEL = level.o levelloader.o
WORLD = world.o bulletevents.o raytracer.o
PLAYER = player.o soldier.o
PHYSICS_OBJECT = physicsobject.o openglmotionstate.o weapon.o

OBJECTFILES = $(addprefix $(OUTPUTDIR)/, $(MAIN) $(GAME) $(MULTIPLAYER) $(LEVEL) $(WORLD) $(PLAYER) $(PHYSICS_OBJECT)) 

### ALL ###

all: server

server: $(OBJECTFILES)
	g++ -o server $(OBJECTFILES) $(LIBS) $(FLAGS) 

### RELEASE ###

release: $(OBJECTFILES)
	g++ -o server $(OBJECTFILES) $(RELEASE) $(LIBS) $(FLAGS)

### MAIN ###

$(OUTPUTDIR)/main.o: $(INPUTDIR)/main.cpp
	g++ -c $(INPUTDIR)/main.cpp -o $@ $(FLAGS)

### GAME ###

$(OUTPUTDIR)/game.o: $(INPUTDIR)/game/game.cpp $(INPUTDIR)/game/game.hpp
	g++ -c $(INPUTDIR)/game/game.cpp -o $@ $(FLAGS)

### MULTIPLAYER ###

$(OUTPUTDIR)/multiplayer.o: $(INPUTDIR)/multiplayer/multiplayer.cpp $(INPUTDIR)/multiplayer/multiplayer.hpp
	g++ -c $(INPUTDIR)/multiplayer/multiplayer.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/node.o: $(INPUTDIR)/multiplayer/node.cpp $(INPUTDIR)/multiplayer/node.hpp
	g++ -c $(INPUTDIR)/multiplayer/node.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/playerdatacollector.o: $(INPUTDIR)/multiplayer/playerdatacollector.cpp $(INPUTDIR)/multiplayer/playerdatacollector.hpp
	g++ -c $(INPUTDIR)/multiplayer/playerdatacollector.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/playerdataupdater.o: $(INPUTDIR)/multiplayer/playerdataupdater.cpp $(INPUTDIR)/multiplayer/playerdataupdater.hpp
	g++ -c $(INPUTDIR)/multiplayer/playerdataupdater.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/physicsobjectdatacollector.o: $(INPUTDIR)/multiplayer/physicsobjectdatacollector.cpp $(INPUTDIR)/multiplayer/physicsobjectdatacollector.hpp
	g++ -c $(INPUTDIR)/multiplayer/physicsobjectdatacollector.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/physicsobjectdataupdater.o: $(INPUTDIR)/multiplayer/physicsobjectdataupdater.cpp $(INPUTDIR)/multiplayer/physicsobjectdataupdater.hpp
	g++ -c $(INPUTDIR)/multiplayer/physicsobjectdataupdater.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/weaponpickercollector.o: $(INPUTDIR)/multiplayer/weaponpickercollector.cpp $(INPUTDIR)/multiplayer/weaponpickercollector.hpp
	g++ -c $(INPUTDIR)/multiplayer/weaponpickercollector.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/weaponpickerupdater.o: $(INPUTDIR)/multiplayer/weaponpickerupdater.cpp $(INPUTDIR)/multiplayer/weaponpickerupdater.hpp
	g++ -c $(INPUTDIR)/multiplayer/weaponpickerupdater.cpp -o $@ $(FLAGS)

### LEVEL ###

$(OUTPUTDIR)/level.o: $(INPUTDIR)/level/level.cpp $(INPUTDIR)/level/level.hpp
	g++ -c $(INPUTDIR)/level/level.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/levelloader.o: $(INPUTDIR)/level/levelloader.cpp $(INPUTDIR)/level/levelloader.hpp
	g++ -c $(INPUTDIR)/level/levelloader.cpp -o $@ $(FLAGS)

### WORLD ###

$(OUTPUTDIR)/world.o: $(INPUTDIR)/world/world.cpp $(INPUTDIR)/world/world.hpp
	g++ -c $(INPUTDIR)/world/world.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/bulletevents.o: $(INPUTDIR)/world/bulletevents.cpp $(INPUTDIR)/world/bulletevents.hpp
	g++ -c $(INPUTDIR)/world/bulletevents.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/raytracer.o: $(INPUTDIR)/world/raytracer.cpp $(INPUTDIR)/world/raytracer.hpp
	g++ -c $(INPUTDIR)/world/raytracer.cpp -o $@ $(FLAGS)

### PLAYER ###

$(OUTPUTDIR)/player.o: $(INPUTDIR)/player/player.cpp $(INPUTDIR)/player/player.hpp
	g++ -c $(INPUTDIR)/player/player.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/soldier.o: $(INPUTDIR)/player/soldier.cpp $(INPUTDIR)/player/soldier.hpp
	g++ -c $(INPUTDIR)/player/soldier.cpp -o $@ $(FLAGS)

### PHYSICS_OBJECT ###

$(OUTPUTDIR)/physicsobject.o: $(INPUTDIR)/physics_object/physicsobject.cpp $(INPUTDIR)/physics_object/physicsobject.hpp
	g++ -c $(INPUTDIR)/physics_object/physicsobject.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/openglmotionstate.o: $(INPUTDIR)/physics_object/openglmotionstate.cpp $(INPUTDIR)/physics_object/openglmotionstate.hpp
	g++ -c $(INPUTDIR)/physics_object/openglmotionstate.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/weapon.o: $(INPUTDIR)/physics_object/weapon.cpp $(INPUTDIR)/physics_object/weapon.hpp
	g++ -c $(INPUTDIR)/physics_object/weapon.cpp -o $@ $(FLAGS)

### CLEAN ###

clean:
	rm -rf $(OUTPUTDIR)/*.o

### CLEANALL ###

cleanall:
	rm -rf server
	rm -rf $(OUTPUTDIR)/*.o
