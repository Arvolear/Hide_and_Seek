#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;
using namespace glm;

class Atmosphere
{
    private:
        RenderQuad* quad;
        Sphere* sphere;
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

        int iBeauty;
        int jBeauty;

        vec3 axis;

    public:
        Atmosphere();
        Atmosphere(vec3 rayOrigin, vec3 sunPos, float sunIntensity, float planetRadius, float atmoRadius, vec3 rayleighCoeff, float mieCoeff, float rayleighHeight, float mieHeight, float mieDir);

        void genBuffer(int width, int height);
        void genBuffer(vec2 size);

        void genDome(vec3 center, float radius, int quality);
        
        void setIBeauty(int iBeauty);
        void setJBeauty(int jBeauty);
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

        void renderAtmosphere(Shader* shader);
        void renderDome(Shader* shader);

        void updateSunPos();

        vec3 getSunPos() const;
        vec3 getSunAxis() const;
        float getRelativeSunGradient() const;

        ColorBuffer* getBuffer() const;
        GLuint getTexture(int index = 0) const;

        ~Atmosphere();
};
