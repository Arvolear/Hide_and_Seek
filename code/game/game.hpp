#pragma once

//openGL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Game
{
    private:
        enum {PLAY, ESCAPE, SHOP};
        int mode;

        Window* window;
        string level;

        //Shader* gameShader; // ESCAPE & SHOP modes, blur...
        FrameBuffer* gameBuffer;
        
        //Level* level;
        //PhysicsWorld* physicsWorld;
        //Player* player
        Camera* camera;

        void checkEvents();

    public:
        Game(Window* window, string level);
        
        void gameLoop();

        ~Game();
};
