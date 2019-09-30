#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Atmosphere
{
    private:
        RenderQuad* quad;
        ColorBuffer* colorBuffer;

        vec3 rayOrigin;
        vec3 sunPos;
        float sunIntensity;
        float planetRadius;
        float atmoRadius;
        vec3 rayleighCoeff;
        float mieCoeff;
        float rayleighHeight;
        float mieHeight;
        float mieDir;

    public:
        Atmosphere();
        Atmosphere(vec3 rayOrigin, vec3 sunPos, float sunIntensity, float planetRadius, float atmoRadius, vec3 rayleighCoeff, float mieCoeff, float rayleighHeight, float mieHeight, float mieDir);
        
        void genBuffer(int width, int height);
        void genBuffer(vec2 size);
        
        void setRayOrigin(vec3 rayOrigin);
        void setSunPos(vec3 sunPos);
        void setSunIntensity(float sunIntensity);
        void setPlanetRadius(float planetRadius);
        void setAtmoRadius(float atmoRadius);
        void setRayleighCoeff(vec3 rayleighCoeff);
        void setMieCoeff(float mieCoeff);
        void setRayleighHeight(float rayleighHeight);
        void setMieHeight(float mieHeight);
        void setMieDir(float mieDir);

        void render(Shader* shader);

        ColorBuffer* getBuffer() const;
        GLuint getTexture(int index = 0) const;

        ~Atmosphere();
};
