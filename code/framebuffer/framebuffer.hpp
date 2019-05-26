#pragma once

#include <stdexcept>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class FrameBuffer
{
    protected:
        GLuint bufferID;
        GLuint textureID;
        
        int width, height;

        FrameBuffer();

    public:
        virtual void genBuffer(double width, double height) = 0;
        virtual void genBuffer(vec2 size) = 0;

        GLuint getBuffer() const;
        GLuint getTexture() const;

        vec2 getSize() const;

        void use();
        virtual void clear() = 0;

        virtual ~FrameBuffer();
};
