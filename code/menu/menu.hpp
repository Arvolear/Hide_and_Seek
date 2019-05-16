#pragma once

//native
#include <iostream>

//openGL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Menu
{
    private:
        Window* window;
        Shader* menuShader;

        FrameBuffer* menuBuffer;

        //Setting* settings
        //Game* game
        
    public:
        Menu();

        void menuLoop();

        ~Menu();
};
