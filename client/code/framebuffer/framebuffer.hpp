#pragma once

#include <stdexcept>
#include <vector>

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
        
        vector < GLuint > texturesID;
        
        int width, height;

        FrameBuffer();

    public:
        virtual void genBuffer(double width, double height, unsigned int layouts = 1) = 0;
        virtual void genBuffer(vec2 size, unsigned int layouts = 1) = 0;

        GLuint getBuffer() const;
        GLuint getTexture(size_t num = 0) const;

        vec2 getSize() const;

        void use();
        void copyColorBuffer(int to, FrameBuffer* frameBuffer, int from);
        void copyDepthBuffer(FrameBuffer* frameBuffer);

        virtual void clear(vec3 color = vec3(0)) = 0;

        virtual ~FrameBuffer();
};
