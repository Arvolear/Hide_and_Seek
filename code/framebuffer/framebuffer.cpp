#include "framebuffer.hpp"

FrameBuffer::FrameBuffer()
{
    bufferID = 0;
    textureID = 0;

    width = height = 0;
}

GLuint FrameBuffer::getBuffer() const
{
    return bufferID;
}

GLuint FrameBuffer::getTexture() const
{
    return textureID;
}

vec2 FrameBuffer::getSize() const
{
    return vec2(width, height);
}

void FrameBuffer::use()
{
    glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
    glViewport(0, 0, width, height);
}

FrameBuffer::~FrameBuffer(){}
