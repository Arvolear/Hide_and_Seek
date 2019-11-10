#include "framebuffer.hpp"
#include "colorbuffer.hpp"

ColorBuffer::ColorBuffer() : FrameBuffer() 
{
    depthBufferID = 0;
}

void ColorBuffer::genBuffer(int width, int height, vector < FrameBufferData > data)
{
    this->width = width;
    this->height = height;

    texturesID.resize(data.size(), 0);

    glGenFramebuffers(1, &bufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, bufferID);

    for (size_t i = 0; i < data.size(); i++)
    {
        glGenTextures(1, &texturesID[i]);
        glBindTexture(GL_TEXTURE_2D, texturesID[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, data[i].internalFormat, width, height, 0, data[i].format, data[i].type, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
   
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texturesID[i], 0);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &depthBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);

    for (size_t i = 0; i < data.size(); i++)
    {
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }

    glDrawBuffers(data.size(), attachments.data());

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        throw runtime_error("ERROR::ColorBuffer");
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ColorBuffer::genBuffer(vec2 size, vector < FrameBufferData > data)
{
    genBuffer(size.x, size.y, data);
}

void ColorBuffer::clear()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ColorBuffer::clearColor(vec4 color)
{
    glClearColor(color.x, color.y, color.z, color.w);

    glClear(GL_COLOR_BUFFER_BIT);
}

void ColorBuffer::clearDepth(float depth)
{
    glClearDepth(depth);

    glClear(GL_DEPTH_BUFFER_BIT);
}

ColorBuffer::~ColorBuffer() 
{
    glDeleteFramebuffers(1, &bufferID);  
    glDeleteRenderbuffers(1, &depthBufferID);  

    for (size_t i = 0; i < texturesID.size(); i++)
    {
        glDeleteTextures(1, &texturesID[i]);
    }
}
