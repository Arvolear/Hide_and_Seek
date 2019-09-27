#include "framebuffer.hpp"

FrameBuffer::FrameBuffer()
{
    bufferID = 0;

    width = height = 0;
}

GLuint FrameBuffer::getBuffer() const
{
    return bufferID;
}

GLuint FrameBuffer::getTexture(size_t num) const
{
    if (num >= texturesID.size())
    {
        throw out_of_range("ERROR::FrameBuffer.getTextures()");
    }

    return texturesID[num];
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
        
void FrameBuffer::copyColorBuffer(int to, FrameBuffer* frameBuffer, int from)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer->getBuffer());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getBuffer());

    glReadBuffer(GL_COLOR_ATTACHMENT0 + from);
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + to);

    int width = getSize().x;
    int height = getSize().y;

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::copyDepthBuffer(FrameBuffer* frameBuffer)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer->getBuffer());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getBuffer());

    int width = getSize().x;
    int height = getSize().y;

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer(){}
