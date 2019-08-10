#pragma once

//opengl
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

template < typename T >
class GaussianBlur
{
    private:
       Shader* blurShader; 

       vector < T* > colorBuffers;
       RenderQuad* quad;

       GLuint bluredTexture;

    public:
        GaussianBlur();

        void genBuffer(float width, float height);
        void genBuffer(vec2 size);

        GLuint blur(GLuint textureID, float intensity, float radius = 1);

        GLuint getTexture() const;

        ~GaussianBlur();

};
