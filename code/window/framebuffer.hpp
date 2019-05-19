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
        
        int width, height;

    public:
        FrameBuffer();

        void genBuffer(double width, double height);
        void genBuffer(vec2 size);

        GLuint getBuffer() const;
        GLuint getDepthBuffer() const;
        GLuint getColorTexture() const;
        //GLuint getDepthTexture();

        vec2 getSize() const;

        ~FrameBuffer();
};
