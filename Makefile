FLAGS = -std=c++11 -Wall -O3

LIBS = -lGLEW -lGL -lGLU -lSOIL -lassimp -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lBulletCollision -lBulletDynamics -lBulletSoftBody -lLinearMath

INPUTDIR = ./code
OUTPUTDIR = ./build

OBJ = main.o shader.o window.o glfwevents.o framebuffer.o renderquad.o menu.o
OBJECTS = $(addprefix $(OUTPUTDIR)/, $(OBJ))

### ALL ###

Hide_and_Seek: $(OBJECTS)
	g++ -o Hide_and_Seek $(OBJECTS) $(LIBS) $(FLAGS) 

### MAIN ###

$(OUTPUTDIR)/main.o: $(INPUTDIR)/main.cpp
	g++ -c $(INPUTDIR)/main.cpp -o $@ $(FLAGS)

### GLOBAL ###

$(OUTPUTDIR)/shader.o: $(INPUTDIR)/global/shader.cpp $(INPUTDIR)/global/shader.hpp
	g++ -c $(INPUTDIR)/global/shader.cpp -o $@ $(FLAGS)

### WINDOW ###

$(OUTPUTDIR)/window.o: $(INPUTDIR)/window/window.cpp $(INPUTDIR)/window/window.hpp
	g++ -c $(INPUTDIR)/window/window.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/glfwevents.o: $(INPUTDIR)/window/glfwevents.cpp $(INPUTDIR)/window/glfwevents.hpp
	g++ -c $(INPUTDIR)/window/glfwevents.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/framebuffer.o: $(INPUTDIR)/window/framebuffer.cpp $(INPUTDIR)/window/framebuffer.hpp
	g++ -c $(INPUTDIR)/window/framebuffer.cpp -o $@ $(FLAGS)

$(OUTPUTDIR)/renderquad.o: $(INPUTDIR)/window/renderquad.cpp $(INPUTDIR)/window/renderquad.hpp
	g++ -c $(INPUTDIR)/window/renderquad.cpp -o $@ $(FLAGS)

### MENU ###

$(OUTPUTDIR)/menu.o: $(INPUTDIR)/menu/menu.cpp $(INPUTDIR)/menu/menu.hpp
	g++ -c $(INPUTDIR)/menu/menu.cpp -o $@ $(FLAGS)

### CLEAN ###

clean:
	rm -rf Hide_and_Seek
	rm -rf $(OUTPUTDIR)/*.o
