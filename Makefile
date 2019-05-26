FLAGS = -std=c++11 -Wall -O3

LIBS = -lGLEW -lGL -lGLU -lSOIL -lassimp -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lBulletCollision -lBulletDynamics -lBulletSoftBody -lLinearMath

INPUTDIR = ./code
OUTPUTDIR = ./build

MAIN = main.o 
DEBUG = debugsphere.o
SHADER = shader.o 
FRAMEBUFFER = framebuffer.o colorbuffer.o depthbuffer.o
WINDOW = window.o glfwevents.o renderquad.o 
MENU = menu.o 
GAME = game.o
LEVEL = level.o dirlight.o skybox.o 
PLAYER = camera.o
GAME_OBJECT = gameobject.o physicsobject.o openglmotionstate.o modelloader.o viewfrustum.o boundsphere.o skeleton.o bone.o mesh.o animation.o

OBJECTFILES = $(addprefix $(OUTPUTDIR)/, $(MAIN) $(DEBUG) $(SHADER) $(FRAMEBUFFER) $(WINDOW) $(MENU) $(GAME) $(LEVEL) $(PLAYER) $(GAME_OBJECT)) 

### ALL ###

Hide_and_Seek: $(OBJECTFILES)
	g++ -o Hide_and_Seek $(OBJECTFILES) $(LIBS) $(FLAGS) 

### MAIN ###

$(OUTPUTDIR)/main.o: $(INPUTDIR)/main.cpp
	g++ -c $(INPUTDIR)/main.cpp -o $@ $(FLAGS)

### DEBUG ###

$(OUTPUTDIR)/debugsphere.o: $(INPUTDIR)/debug/debugsphere.cpp $(INPUTDIR)/debug/debugsphere.hpp
	g++ -c $(INPUTDIR)/debug/debugsphere.cpp -o $@ $(FLAGS)

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

### LEVEL ###

$(OUTPUTDIR)/level.o: $(INPUTDIR)/level/level.cpp $(INPUTDIR)/level/level.hpp
	g++ -c $(INPUTDIR)/level/level.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/dirlight.o: $(INPUTDIR)/level/dirlight.cpp $(INPUTDIR)/level/dirlight.hpp
	g++ -c $(INPUTDIR)/level/dirlight.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/skybox.o: $(INPUTDIR)/level/skybox.cpp $(INPUTDIR)/level/skybox.hpp
	g++ -c $(INPUTDIR)/level/skybox.cpp -o $@ $(FLAGS)

### PLAYER ###

$(OUTPUTDIR)/camera.o: $(INPUTDIR)/player/camera.cpp $(INPUTDIR)/player/camera.hpp
	g++ -c $(INPUTDIR)/player/camera.cpp -o $@ $(FLAGS)

### GAME_OBJECT ###

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

### CLEAN ###

clean:
	rm -rf Hide_and_Seek
	rm -rf $(OUTPUTDIR)/*.o
