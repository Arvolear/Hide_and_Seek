#include "../shader/shader.hpp"

#include "framebuffer.hpp"
#include "gbuffer.hpp"

GBuffer::GBuffer() : FrameBuffer() 
{
    depthBufferID = 0;
}

void GBuffer::genBuffer(double width, double height, unsigned int layouts)
{
    this->width = width;
    this->height = height;

    texturesID.resize(layouts, 0);

    glGenFramebuffers(1, &bufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, bufferID);

    /* position */
    glGenTextures(1, &texturesID[0]);
    glBindTexture(GL_TEXTURE_2D, texturesID[0]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texturesID[0], 0);

    /* normal */
    glGenTextures(1, &texturesID[1]);
    glBindTexture(GL_TEXTURE_2D, texturesID[1]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texturesID[1], 0);

    /* albedo */
    glGenTextures(1, &texturesID[2]);
    glBindTexture(GL_TEXTURE_2D, texturesID[2]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texturesID[2], 0);

    /* metallic */
    glGenTextures(1, &texturesID[3]);
    glBindTexture(GL_TEXTURE_2D, texturesID[3]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, texturesID[3], 0);
    
    /* roughness */
    glGenTextures(1, &texturesID[4]);
    glBindTexture(GL_TEXTURE_2D, texturesID[4]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, texturesID[4], 0);
    
    /* AO */
    glGenTextures(1, &texturesID[5]);
    glBindTexture(GL_TEXTURE_2D, texturesID[5]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, texturesID[5], 0);
    
    /* light scattering */
    glGenTextures(1, &texturesID[6]);
    glBindTexture(GL_TEXTURE_2D, texturesID[6]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, texturesID[6], 0);
    
    glBindTexture(GL_TEXTURE_2D, 0);

    /* depth buffer */
    glGenRenderbuffers(1, &depthBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);

    vector < unsigned int > attachments;

    for (size_t i = 0; i < layouts; i++)
    {
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }

    glDrawBuffers(layouts, attachments.data());

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        throw runtime_error("ERROR::gBuffer");
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::genBuffer(vec2 size, unsigned int layouts)
{
    genBuffer(size.x, size.y, layouts);
}

void GBuffer::clear()
{
    glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::render(Shader* shader)
{
    /* position */
    glActiveTexture(GL_TEXTURE0 + texturesID[0]);
    glBindTexture(GL_TEXTURE_2D, texturesID[0]);
    shader->setInt("gBuffer.texture_position", texturesID[0]);

    /* normal */
    glActiveTexture(GL_TEXTURE0 + texturesID[1]);
    glBindTexture(GL_TEXTURE_2D, texturesID[1]);
    shader->setInt("gBuffer.texture_normal", texturesID[1]);

    /* albedo */
    glActiveTexture(GL_TEXTURE0 + texturesID[2]);
    glBindTexture(GL_TEXTURE_2D, texturesID[2]);
    shader->setInt("gBuffer.texture_albedo", texturesID[2]);
    
    /* metallic */
    glActiveTexture(GL_TEXTURE0 + texturesID[3]);
    glBindTexture(GL_TEXTURE_2D, texturesID[3]);
    shader->setInt("gBuffer.texture_metallic", texturesID[3]);
    
    /* roughness */
    glActiveTexture(GL_TEXTURE0 + texturesID[4]);
    glBindTexture(GL_TEXTURE_2D, texturesID[4]);
    shader->setInt("gBuffer.texture_roughness", texturesID[4]);
    
    /* AO */
    glActiveTexture(GL_TEXTURE0 + texturesID[5]);
    glBindTexture(GL_TEXTURE_2D, texturesID[5]);
    shader->setInt("gBuffer.texture_ao", texturesID[5]);
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
