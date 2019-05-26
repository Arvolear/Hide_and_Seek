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

        //Shader* gameShader; // ESCAPE & SHOP modes, blur...
        ColorBuffer* gameBuffer;
        
        Level* level;
        string levelName;
        //PhysicsWorld* physicsWorld;

        //Player* player
        Camera* camera;

        void checkEvents();

    public:
        Game(Window* window, string level);
        
        void gameLoop();

        ~Game();
};
