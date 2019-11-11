#include "../shader/shader.hpp"

#include "framebuffer.hpp"
#include "gbuffer.hpp"

GBuffer::GBuffer() : FrameBuffer() 
{
    depthBufferID = 0;
}

void GBuffer::genBuffer(int width, int height, vector < FrameBufferData > data)
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texturesID[i], 0);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    /* depth buffer */
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

        throw runtime_error("ERROR::gBuffer");
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::genBuffer(vec2 size, vector < FrameBufferData > data)
{
    genBuffer(size.x, size.y, data);
}

void GBuffer::clear()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::clearColor(vec4 color)
{
    glClearColor(color.x, color.y, color.z, color.w);

    glClear(GL_COLOR_BUFFER_BIT);
}

void GBuffer::clearDepth(float depth)
{
    glClearDepth(depth);

    glClear(GL_DEPTH_BUFFER_BIT); 
}

void GBuffer::render(Shader* shader)
{
    /* position */
    glActiveTexture(GL_TEXTURE0);
    shader->setInt("gBuffer.texture_position", 0);
    glBindTexture(GL_TEXTURE_2D, texturesID[0]);

    /* normal */
    glActiveTexture(GL_TEXTURE0 + 1);
    shader->setInt("gBuffer.texture_normal", 1);
    glBindTexture(GL_TEXTURE_2D, texturesID[1]);

    /* albedo */
    glActiveTexture(GL_TEXTURE0 + 2);
    shader->setInt("gBuffer.texture_albedo", 2);
    glBindTexture(GL_TEXTURE_2D, texturesID[2]);
    
    /* met rough ao */
    glActiveTexture(GL_TEXTURE0 + 3);
    shader->setInt("gBuffer.texture_metRoughAOCos", 3);
    glBindTexture(GL_TEXTURE_2D, texturesID[3]);
    
    /* static depth */
    glActiveTexture(GL_TEXTURE0 + 4);
    shader->setInt("gBuffer.texture_staticDepth", 4);
    glBindTexture(GL_TEXTURE_2D, texturesID[7]);
}
        
void GBuffer::renderSsao(Shader* shader)
{
    /* ssao position */
    glActiveTexture(GL_TEXTURE0);
    shader->setInt("gBuffer.texture_position", 0);
    glBindTexture(GL_TEXTURE_2D, texturesID[5]);

    /* ssao normal */
    glActiveTexture(GL_TEXTURE0 + 1);
    shader->setInt("gBuffer.texture_normal", 1);
    glBindTexture(GL_TEXTURE_2D, texturesID[6]);
}

void GBuffer::renderStaticDepth(Shader* shader)
{
    /* static depth */
    glActiveTexture(GL_TEXTURE0);
    shader->setInt("depthTexture", 0);
    glBindTexture(GL_TEXTURE_2D, texturesID[7]);
}
        
GBuffer::~GBuffer() 
{
    glDeleteFramebuffers(1, &bufferID);  
    glDeleteRenderbuffers(1, &depthBufferID);  

    for (size_t i = 0; i < texturesID.size(); i++)
    {
        glDeleteTextures(1, &texturesID[i]);
    }
}
