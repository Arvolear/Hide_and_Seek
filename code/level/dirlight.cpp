#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"

#include "dirlight.hpp"

DirLight::DirLight()
{
    direction = vec3(0.0);
    ambient = vec3(0.0);
    diffuse = vec3(0.0);
    specular = vec3(0.0);

    view = mat4(1.0);
    projection = mat4(1.0);

    shadowBuffer = new DepthBuffer();
}

DirLight::DirLight(vec3 dir, vec3 amb, vec3 diff, vec3 spec)
{
    direction = dir;

    ambient = amb;
    diffuse = diff;
    specular = spec;

    shadowBuffer = new DepthBuffer();
}

void DirLight::genShadowBuffer(int width, int height)
{
    shadowBuffer->genBuffer(width, height);
}

void DirLight::genShadowBuffer(vec2 size)
{
    shadowBuffer->genBuffer(size);
}

void DirLight::setDirection(vec3 dir)
{
    direction = dir;
}

void DirLight::setAmbient(vec3 amb)
{
    ambient = amb;
}

void DirLight::setDiffuse(vec3 diff)
{
    diffuse = diff;
}

void DirLight::setSpecular(vec3 spec)
{
    specular = spec;
}

vec3 DirLight::getDirection() const
{
    return direction;
}

void DirLight::setView(mat4 view)
{
    this->view = view;
}

void DirLight::setProjection(mat4 projection)
{
    this->projection = projection;
}

void DirLight::render(Shader* shader, GLuint index)
{
    glUniform3f(glGetUniformLocation(shader->getID(), ("dirLights[" + to_string(index) + "].direction").c_str()), direction.x, direction.y, direction.z);
    glUniform3f(glGetUniformLocation(shader->getID(), ("dirLights[" + to_string(index) + "].ambient").c_str()), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(shader->getID(), ("dirLights[" + to_string(index) + "].diffuse").c_str()), diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(shader->getID(), ("dirLights[" + to_string(index) + "].specular").c_str()), specular.x, specular.y, specular.z);
    
    glActiveTexture(GL_TEXTURE0 + index + 10);
    glBindTexture(GL_TEXTURE_2D, shadowBuffer->getTexture());
    glUniform1i(glGetUniformLocation(shader->getID(), ("dirLights[" + to_string(index) + "].texture_shadow1").c_str()), index + 10);
}
        
DepthBuffer* DirLight::getShadowBuffer() const
{
    return shadowBuffer;
}

void DirLight::updateView(vec3 playerPosition)
{
    view = lookAt(playerPosition, playerPosition + direction, vec3(0, 1, 0));
}

mat4 DirLight::getView() const
{
    return view;
}

mat4 DirLight::getProjection() const
{
    return projection;
}

DirLight::~DirLight()
{
    delete shadowBuffer;
}
