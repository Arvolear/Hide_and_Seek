#pragma once

//opengl
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GaussianBlur
{
    private:
       Shader* blurShader; 

       vector < ColorBuffer* > colorBuffers;
       RenderQuad* quad;

    public:
        GaussianBlur(Window* window);

        GLuint blur(GLuint textureID, float intensity);

        ~GaussianBlur();

};
