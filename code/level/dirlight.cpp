#include "../shader/shader.hpp"

#include "dirlight.hpp"

DirLight::DirLight()
{
    direction = vec3(0);
    ambient = vec3(0);
    diffuse = vec3(0);
    specular = vec3(0);
}

DirLight::DirLight(vec3 dir, vec3 amb, vec3 diff, vec3 spec)
{
    direction = dir;

    ambient = amb;
    diffuse = diff;
    specular = spec;
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

void DirLight::render(Shader* shader)
{
    glUniform3f(glGetUniformLocation(shader->getID(), "dirLight.direction"), direction.x, direction.y, direction.z);
    glUniform3f(glGetUniformLocation(shader->getID(), "dirLight.ambient"), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(shader->getID(), "dirLight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(shader->getID(), "dirLight.specular"), specular.x, specular.y, specular.z);
}

DirLight::~DirLight(){}
