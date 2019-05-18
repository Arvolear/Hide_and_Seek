#pragma once

//native
#include <iostream>

//opengl
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class FrameBuffer
{
    private:
        GLuint bufferID;
        GLuint depthBufferID;

        GLuint colorTextureID;
        //GLuint depthTextureID;

    public:
        FrameBuffer();

        void genBuffer(double width, double height);

        GLuint getBuffer() const;
        GLuint getDepthBuffer() const;
        GLuint getColorTexture() const;
        //GLuint getDepthTexture();

        ~FrameBuffer();
};
