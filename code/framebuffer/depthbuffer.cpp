#include "framebuffer.hpp"
#include "depthbuffer.hpp"

DepthBuffer::DepthBuffer() : FrameBuffer() {}

void DepthBuffer::genBuffer(double width, double height, unsigned int layouts)
{
    this->width = width;
    this->height = height;

    texturesID.resize(1, 0);

    glGenFramebuffers(1, &bufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, bufferID);

    glGenTextures(1, &texturesID[0]);
    glBindTexture(GL_TEXTURE_2D, texturesID[0]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texturesID[0], 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        throw runtime_error("ERROR::DepthBuffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

void DepthBuffer::genBuffer(vec2 size, unsigned int layouts)
{
    genBuffer(size.x, size.y, layouts);
}

void DepthBuffer::clear()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

DepthBuffer::~DepthBuffer() 
{
    glDeleteFramebuffers(1, &bufferID);  
    
    for (size_t i = 0; i < texturesID.size(); i++)
    {
        glDeleteTextures(1, &texturesID[i]);
    }
} 
