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

struct FrameBufferData
{
    GLint internalFormat;
    GLenum format;
    GLenum type;
};

class FrameBuffer
{
    protected:
        GLuint bufferID;
        
        vector < GLuint > texturesID;
        vector < unsigned int > attachments;
        
        int width, height;

        FrameBuffer();

    public:
        virtual void genBuffer(int width, int height, vector < FrameBufferData > data) = 0;
        virtual void genBuffer(vec2 size, vector < FrameBufferData > data) = 0;

        GLuint getBuffer() const;
        GLuint getTexture(size_t num = 0) const;

        vec2 getSize() const;

        void use();
        void useDepth();
        void copyColorBuffer(int to, FrameBuffer* frameBuffer, int from);
        void copyDepthBuffer(FrameBuffer* frameBuffer);

        virtual void clear(vec4 colorDepth = vec4(0.0, 0.0, 0.0, 1.0)) = 0;

        virtual ~FrameBuffer();
};
